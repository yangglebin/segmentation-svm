#include <map>

#ifndef __fastm_common_h__
#define __fastm_common_h__

#define GET(_img,_i,_j,_k) (_img[(_i)*nk*nj + (_j)*nk + (_k)])
#define SET(_img,_i,_j,_k,v) (_img[(_i)*nk*nj + (_j)*nk + (_k)] = (v))
#define GETID(_i,_j,_k) ((_i)*nk*nj + (_j)*nk + (_k))
#define GETSUBi(_sub) (_sub/nk/nj)
#define GETSUBj(_sub) ((_sub/nk)%nj)
#define GETSUBk(_sub) (_sub%nk)


// #define NEIGHBORHOOD_SIZE(i) ((i==0)?6:26)

///-----------------------------------------------------------------------------

/** allocator
 */
// typedef void*(*Allocator)(int, unsigned int*, char const*, char const*);
typedef void*(*Allocator)(int, unsigned int*, char*, char*);

///-----------------------------------------------------------------------------
/** neighborhood types
 */

static int neighborhood_size[2] = {6, 26};
static int neighborhood6[6][3] = {{-1,0,0},{0,-1,0},{0,0,-1},{1,0,0},{0,1,0},{0,0,1}};
static int neighborhood26[26][3] = {
    {-1,-1,-1}, {-1,-1, 0}, {-1,-1, 1},
    {-1, 0,-1}, {-1, 0, 0}, {-1, 0, 1},
    {-1, 1,-1}, {-1, 1, 0}, {-1, 1, 1},
    
    { 0,-1,-1}, { 0,-1, 0}, { 0,-1, 1},
    { 0, 0,-1},             { 0, 0, 1},
    { 0, 1,-1}, { 0, 1, 0}, { 0, 1, 1},
    
    { 1,-1,-1}, { 1,-1, 0}, { 1,-1, 1},
    { 1, 0,-1}, { 1, 0, 0}, { 1, 0, 1},
    { 1, 1,-1}, { 1, 1, 0}, { 1, 1, 1}
    };



///-----------------------------------------------------------------------------

/** Arrival Time class prototype
 */
class ArrivalTime
{
    public:
      /** functions to compute distance */
      float operator() (unsigned int i,
                        unsigned int j,
                        unsigned int k) const
      { return this->compute_time(i,j,k,0,0,0); }
      
      float operator()(unsigned int i,
                       unsigned int j,
                       unsigned int k,
                       unsigned int oi,
                       unsigned int oj,
                       unsigned int ok) const
      { return this->compute_time(i,j,k,oi,oj,ok); }
      

      /** to overload  */
      virtual float compute_time(unsigned int i,
                                 unsigned int j,
                                 unsigned int k,
                                 unsigned int oi,
                                 unsigned int oj,
                                 unsigned int ok) const =0;
      /** to overload */
      virtual float compute_interval(unsigned int i1,
                                     unsigned int j1,
                                     unsigned int k1,
                                     unsigned int i2,
                                     unsigned int j2,
                                     unsigned int k2) const =0;
      
      /** to overload */
      virtual float compute_delta(unsigned int heap_size) const =0;
      
      /** constructor */
      ArrivalTime(unsigned int const size[3], float offset,
              float const *I, int const *Q, float const *D, 
              int neighborhood_type=0)
        : _I(I), _Q(Q), _D(D), _offset(offset), _neigh_type(neighborhood_type),
          ni(size[0]), nj(size[1]), nk(size[2]) {}
          
  protected:
    int const *_Q;          /// index image
    float const *_D;        /// arrival time image
    float const *_I;        /// image
    float _offset;          /// speed offset
    int _neigh_type;        /// neighborhood type
    unsigned int ni,nj,nk;  /// image size
};
 
///-----------------------------------------------------------------------------

struct Cell
{
    int &q;     /// state: (-2)=unknown, (-1)=narrow band, (0+)=label (frozen)
    float &d;   /// distance
    unsigned int i,j,k; /// coordinate
    unsigned int label; /// label to assign when freezing
    float center;
    
    /** constructor */
    Cell(int &_q, float &_d, unsigned int _label,
         unsigned int _i, unsigned int _j, unsigned int _k, 
         float _center=0)
    : q(_q),d(_d),label(_label),
      i(_i),j(_j),k(_k),
      center(_center) {}
    
    /** copy constructor */
    Cell( Cell const &other)
    : q(other.q),d(other.d),label(other.label),
      i(other.i),j(other.j),k(other.k), 
      center(other.center) {}
};
///-----------------------------------------------------------------------------

/** Edge data container
 */
template <typename T>
class EdgeData
{
  public:
    typedef std::pair<int, T> Pair;
    typedef std::map<int, T> Map;
    
    /** constructor: needs number of nodes
     */
    EdgeData(int nnode): _nnode(nnode){}
    
    /** insert or change edge data
     */
    void insert(int node1, int node2, T data)
    {   
        if (node1<node2)      _data_map[node1*_nnode + node2] = data;
        else if (node2<node1) _data_map[node2*_nnode + node1] = data;
    }
    
    /** test if edge exists
     */
    bool has(int node1, int node2) const
    {
        typename Map::const_iterator pt = _data_map.end();
        if (node1<node2) pt = _data_map.find(node1*_nnode + node2);
        else if (node2<node1) pt = _data_map.find(node2*_nnode + node1);
        
        if (pt != _data_map.end()) return true;
        else return false;
    }
    
    /** get edge value
     */
    T get(int node1, int node2) const
    {
        typename Map::const_iterator pt = _data_map.end();
        if (node1<node2) pt = _data_map.find(node1*_nnode + node2);
        else if (node2<node1) pt = _data_map.find(node2*_nnode + node1);
        
        if (pt != _data_map.end()) return (pt->second);
        else return (T)0;
    }
    
    /** return nuber of edges
     */
    int number_of_edges() const {return _data_map.size();}
    
    /** return data
     */
    template<class ItData>
    void get_data(ItData itd) const
    {
        typename Map::const_iterator m;
        for (m = _data_map.begin(); m!=_data_map.end(); ++m, ++itd)
        {
            (*itd) = (*m).second;
        }
    }
    
    /** return edges
     */
    template<class ItEdge>
    void get_edges(ItEdge ite) const
    {
        typename Map::const_iterator m;
        for (m = _data_map.begin(); m!=_data_map.end(); ++m, ++ite)
        {
            (*ite) = (*m).first/_nnode;
            (*(++ite)) = (*m).first%_nnode;
        }
    }
    
  private:
    int _nnode;
    Map _data_map;
};
///-----------------------------------------------------------------------------


#endif
