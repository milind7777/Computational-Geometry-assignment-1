#include<bits/stdc++.h>
#define pb push_back

using namespace std;

#include <CGAL/Cartesian.h>
#include <CGAL/MP_Float.h>
#include <CGAL/Quotient.h>
#include <CGAL/Exact_rational.h>
#include <CGAL/Arr_segment_traits_2.h>
#include <CGAL/Arrangement_2.h>
#include <CGAL/Arr_extended_dcel.h>
#include <CGAL/Arr_overlay_2.h>
#include <CGAL/Arr_default_overlay_traits.h>

typedef CGAL::Quotient<CGAL::MP_Float> Number_type;
typedef CGAL::Cartesian<Number_type> Kernel;
typedef CGAL::Arr_segment_traits_2<Kernel> Traits_2;
typedef Traits_2::Point_2 Point_2;
typedef Traits_2::X_monotone_curve_2 Segment_2;
typedef CGAL::Arr_face_extended_dcel<Traits_2, int> Dcel;
typedef CGAL::Arrangement_2<Traits_2, Dcel> Arrangement_2;
typedef CGAL::Arr_face_overlay_traits<Arrangement_2,
                                      Arrangement_2,
                                      Arrangement_2,
                                      std::plus<int> > Overlay_traits;

Arrangement_2 constructDCEL(string s){
    ifstream inp; inp.open(s);
    vector<Point_2> vertex;
    vector<Segment_2> segment;

    float x, y; int a, b; string line;
    bool f = 0;
    
    while(getline(inp, line)){
        //cout << line << endl;
        
        if(line == "*****"){
            f = 1; continue;
        }

        stringstream l(line);
        if(f==0){ // process vectices
            l >> x >> y;
            Point_2 point (x, y);
            vertex.pb(point);
        } else {  // process segments
            l >> a >> b;
            segment.pb(Segment_2 (vertex[a-1], vertex[b-1]));
        }
    }

    Arrangement_2 polygon;
    CGAL::insert (polygon, segment.begin(), segment.end());

    return polygon;
}

void printOverlayFaces(Arrangement_2 overlay, int faceid, bool inc){ 
    // if inc == 1 then faces with id == faceid are considered
    // if inc == 0 then faces with id != faceid are considered

    Arrangement_2::Face_iterator             f;
    Arrangement_2::Ccb_halfedge_circulator   e;

    int face_num = 1;
    for(f=overlay.faces_begin(); f!=overlay.faces_end(); f++){
        if(inc == 1 and f->data() != faceid) continue;
        if(inc == 0 and f->data() == faceid) continue;    
        //cout << f->data() << endl;

        cout << endl;
        cout << "face " << face_num << " : "; face_num++;
        e = f->outer_ccb();
        cout << "(" << e->source()->point() << ")";
        do{
            cout << " --> (" << e->target()->point() << ")"; e++;
        } while(e != f->outer_ccb());

        cout << endl;
    }

    cout << endl;

}

int main(int argc, char * argv[]){
    
    // Construct DCEL of the input polygons
    Arrangement_2 polygon1 = constructDCEL(argv[1]);
    Arrangement_2 polygon2 = constructDCEL(argv[2]);

    // for polygon1 we set face id to 01 and for polygon2 we set it to 10 for the 
    // interior face.
    // So when compute overlay we perform the PLUS operation which gives:
    // 01+10 = 11 intersection of both polygons
    // 01+00 = 01 only polygon 1.
    // 00+10 = 10 only polygon 2.
    // 00+00 = 00 none of them.
    Arrangement_2::Face_iterator   fit;
    for (fit = polygon1.faces_begin(); fit != polygon1.faces_end(); ++fit)
        if(fit != polygon1.unbounded_face()) fit->set_data(01);
        else fit->set_data(0);  

    for (fit = polygon2.faces_begin(); fit != polygon2.faces_end(); ++fit)
        if(fit != polygon2.unbounded_face()) fit->set_data(10);
        else fit->set_data(0);
    
    // Compute the overlay of the two arrangements.
    Arrangement_2          overlay_arr;
    Overlay_traits         overlay_traits;
    overlay (polygon1, polygon2, overlay_arr, overlay_traits);
    // Print the size of the overlaid arrangement.
    std::cout << "The overlay arrangement size: " << std::endl
            << "   V = " << overlay_arr.number_of_vertices()
            << ",  E = " << overlay_arr.number_of_edges()
            << ",  F = " << overlay_arr.number_of_faces() << std::endl;
    cout << endl;

    cout << "The union of polygons is given by faces :\n";
    printOverlayFaces(overlay_arr, 0, 0);

    cout << "The intersection of polygons is given by faces :\n";
    printOverlayFaces(overlay_arr, 11, 1);

    cout << "The difference of polygons 1 and 2 is given by faces :\n";
    printOverlayFaces(overlay_arr, 1, 1);

    return 0;
}