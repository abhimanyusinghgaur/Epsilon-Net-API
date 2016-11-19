#ifndef enet_h__
#define enet_h__

/*
*  This program is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*  Release date: 01/09/2015
*  Authors: Norbert Bus, Nabil H. Mustafa*  
*  Description: this module provides a function to compute epsilon-nets 
*  of small size for disks in the plane: 
*
*  Update (25/10/2016) : Changed the signature of the function to avoid gcc compilation error (default value for non const reference).
*
*  If you find this code useful, please cite it by referencing the following paper:
*  @article{BGMR15, title={Tighter Estimates for epsilon-nets for Disks}, author={N. Bus and S. Garg and N. H. Mustafa and S. Ray}}
*/

#include "CGAL/Exact_predicates_inexact_constructions_kernel.h"
#include "CGAL/Triangulation_vertex_base_with_info_2.h"
#include "CGAL/Triangulation_face_base_with_info_2.h"
#include "CGAL/Delaunay_triangulation_2.h"
#include "CGAL/Circle_2.h"

namespace enet{

  ////////////////////////////////// typedefs for CGAL  ///////////////////////////////////////

  struct FaceInfo { 
    std::vector<size_t> diskpoints; 
  };

  typedef CGAL::Exact_predicates_inexact_constructions_kernel  K;

  typedef CGAL::Circle_2<K> Circle;
  typedef CGAL::Vector_2<K> Vector;

  typedef CGAL::Triangulation_face_base_2<K> Tfb;
  typedef CGAL::Triangulation_vertex_base_2<K> Tvb;
  typedef CGAL::Triangulation_data_structure_2<Tvb, Tfb> Tds;
  typedef CGAL::Delaunay_triangulation_2<K, Tds> Delaunay;

  typedef CGAL::Triangulation_face_base_with_info_2<FaceInfo, K> TfbInfo;
  typedef CGAL::Triangulation_vertex_base_with_info_2<size_t, K> TvbInfo;
  typedef CGAL::Triangulation_data_structure_2<TvbInfo, TfbInfo> TdsInfo;
  typedef CGAL::Delaunay_triangulation_2<K, TdsInfo> DelaunayInfo;
  typedef DelaunayInfo::Point Point;
  typedef DelaunayInfo::Edge Edge;
  typedef DelaunayInfo::Face_handle Face_handle;
  typedef DelaunayInfo::Vertex_handle Vertex_handle;
  typedef DelaunayInfo::Face_circulator Face_circulator;


  ////////////////////////////////// enet functions ///////////////////////////////////////

  extern double eps_c;  //probability constant (c/eps) for random sample in the eps-net algo

  /*!
  * \brief Construct eps-net with random sampling.
  * \param           input_points  The input points.
  * \param [in, out] weights       The weights.
  * \param           epsilon       The epsilon.
  * \param [out]     epsnet        The epsnet.
  * \param [out]     counter       The counter for epsnet points not directly calculated (only used until a proper implementation for epsilon>0.5 is added).
  * \param [in]      currentPoints The current points, to denote that only a subset of points should be considered.
  * return size of first random sample
  */
  size_t randomSampleEpsnet(const std::vector<Point> & input_points, const std::vector<double> & weights, const double epsilon, std::vector<size_t>& epsnet, size_t& counter, std::vector<size_t>* pCurrentPoints = NULL );

}
#endif // epsnet_h__
