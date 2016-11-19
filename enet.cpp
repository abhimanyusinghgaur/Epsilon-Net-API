#include <algorithm>
#include <time.h>
#include "CGAL/Random.h"
#include <CGAL/bounding_box.h>
#include <stdlib.h>
#include "./enet.h"
//#include "header/timer.h"

namespace enet{

  double eps_c = 10.0; // parameter for eps-net construction (theoretically 12 is good, experimentally 7 seems good)


  size_t randomSampleEpsnet( const std::vector<Point> & input_points, const std::vector<double> & weights, const double epsilon, std::vector<size_t>& epsnet, size_t& counter,std::vector<size_t>* pCurrentPoints /*= NULL*/ )
  {
    // currentpoints contains the indices of points in the current problem (if empty we take all the points)
    bool deleteCurrentPoints = false;
    if (!pCurrentPoints)
    {
      deleteCurrentPoints = true;
      pCurrentPoints = new std::vector<size_t>();
      pCurrentPoints->reserve(input_points.size());
      for (size_t i = 0 ; i < input_points.size() ; ++i )
        pCurrentPoints->push_back(i);
    }
    std::vector<size_t>& currentPoints = *pCurrentPoints;

    // set some parameters as in the paper
    const size_t n = currentPoints.size();
    // const double c = 3.0;
    const double probability = eps_c / epsilon  ;
    // setup the variables holding the sample etc.
    const size_t expectedSize = std::min( n , static_cast<size_t>( eps_c / epsilon ) );
    std::vector<Point> randomSample; randomSample.reserve( expectedSize );
    std::vector<Point> remainingPoints; remainingPoints.reserve( n - expectedSize );
    std::vector<size_t> tmp; tmp.reserve( n - expectedSize );
    double totalWeight = 0.0;
    for (size_t i = 0 ; i < n ; ++i)
      totalWeight +=  weights[currentPoints[i]];

    // create random samples and push them back as epsnet points
    // store the remaining points as well 
    // CGAL::Random rnd( getSeed() ); with a custom macro to get milisec time
    CGAL::Random rnd( time(NULL) );
    for (size_t i = 0; i < n ; ++i)
    {
      double prob = probability *  weights[currentPoints[i]] / totalWeight;
      if ( prob > rnd.get_double() )
      {
        randomSample.push_back(input_points[currentPoints[i]]);
        epsnet.push_back(currentPoints[i]);
      }
      else
      {
        remainingPoints.push_back(input_points[currentPoints[i]]);
        tmp.push_back(currentPoints[i]);
      }
    }
    // WARNING: this degenerate case should be handled properly
    if ( randomSample.size() < 3 )
    {
      for (size_t i = 0; i < n ; ++i)
        epsnet.push_back(currentPoints[i]);
      return n; 
    }

    // Create the Delaunay triangulation for the random sample
    std::vector< std::pair<Point,unsigned> > inputPoints_w_info; 
    for (size_t i=0;i<randomSample.size();++i)
      inputPoints_w_info.push_back( std::make_pair(randomSample[i], (unsigned) i ) );
    DelaunayInfo dt(inputPoints_w_info.begin(), inputPoints_w_info.end());
    //std::cout << "Number of Delaunay vertices: " <<dt.number_of_vertices() << std::endl;

    // locate for all remaining points the disks that contain it
    CGAL::spatial_sort(remainingPoints.begin(),remainingPoints.end());
    Face_handle face;
    for (size_t i=0; i < remainingPoints.size(); ++i)
    {
      face = dt.locate(remainingPoints[i],face);
      // get conflict list and add points;
      std::vector<Face_handle> containingDisks;
      dt.get_conflicts(remainingPoints[i], std::back_inserter(containingDisks),face); 
      for (size_t j = 0; j < containingDisks.size() ; ++j)
        containingDisks[j]->info().diskpoints.push_back(tmp[i]);
    }

    // for all faces sort the lists of points
    DelaunayInfo::All_faces_iterator faces = dt.all_faces_begin();
    for ( ; faces != dt.all_faces_end(); ++faces)
      std::sort (faces->info().diskpoints.begin(),faces->info().diskpoints.end());
    // for each edge take the two disks and merge the points in them
    //   for each subproblem call a proper function
    DelaunayInfo::All_edges_iterator edges = dt.all_edges_begin ();
    for ( ; edges != dt.all_edges_end(); ++edges)
    {
      Edge e=*edges;
      Face_handle face1 = e.first;
      Face_handle face2 = e.first->neighbor(e.second);
      size_t subproblemSize = face1->info().diskpoints.size() + face2->info().diskpoints.size();
      // if they are empty continue
      if (subproblemSize == 0)
        continue;
      // simply union the two lists since they are already sorted
      std::vector<size_t> subproblem; subproblem.reserve(subproblemSize);
      std::set_union (
        face1->info().diskpoints.begin(), face1->info().diskpoints.end(),
        face2->info().diskpoints.begin(), face2->info().diskpoints.end(),
        std::back_inserter(subproblem));
      // according to the subproblem size call the proper function to solve the problem
      double subproblemWeight = 0.0;
      for (size_t i = 0 ;  i < subproblem.size() ; ++i)
        subproblemWeight +=  weights[subproblem[i]];

      // the algorithms for constructing small sized epsnets for subproblemEpsilon>0.5 have not been implemented
      // instead we call the function recursively
      double subproblemEpsilon = epsilon * totalWeight / subproblemWeight;
      if (subproblemEpsilon > 1.0)  // 0 points are needed
      {
        continue;
      }
      else if (subproblemEpsilon > 2.0/3.0)  // 2 points are needed
      {
        randomSampleEpsnet(input_points, weights, subproblemEpsilon, epsnet, counter, &subproblem);
        //counter += 2;
      }
      else if (subproblemEpsilon > 0.5)  // 10 points are needed
      {
        randomSampleEpsnet(input_points, weights, subproblemEpsilon, epsnet, counter, &subproblem);
        //counter+=10;
      }
      else               // we have a proper subproblem
        randomSampleEpsnet(input_points, weights, subproblemEpsilon, epsnet, counter, &subproblem);
    }

    if (deleteCurrentPoints) delete pCurrentPoints;

    return randomSample.size();
  }

}

using namespace std;
using namespace enet;

int main(int argc, const char* argv[])
{
  //argv: {"binary file name", "epsilon value", "x1", "y1", "x2", "y2", ....}
  if(argc < 4) {
    cerr<<"ERROR: Insufficient arguments\n";
    exit(EXIT_FAILURE);
  }

  vector<Point> input_points;
  vector<double> weights( argc/2 - 1, 1.0);
  double epsilon = atof(argv[1]);
  vector<size_t> epsnet;
  size_t counter;

  for(int i=2; i<argc; i+=2) {
    input_points.push_back(Point(atoi(argv[i]), atoi(argv[i+1])));
  }

  //  Check this case as delauney triangulation is not possible in this case
  if(input_points.size() < 3) {
    for(int i=0; i<input_points.size(); i++)
      cout<<i<<" ";
    return 0;
  }

  do {
    epsnet.clear();
    randomSampleEpsnet(input_points, weights, epsilon, epsnet, counter);
  } while(epsnet.size() > 13.4/epsilon);

  for(int i=0; i<epsnet.size(); i++)
    cout<<epsnet[i]<<" ";

  return 0;
}