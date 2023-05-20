#include<bits/stdc++.h>
#include<fstream>
#include "bitmap_image.hpp"

using namespace std;

void mid_point_line(int x_start, int y_start, int x_end, int y_end, int r, int g, int b, bitmap_image &image, int width, int height)
{
    //(0,0) is bottom left corner
    //(W-1, H-1) is top right corner

    if(x_end < x_start)
    {
        swap(x_start, x_end);
        swap(y_start, y_end);
    }

    
    int dx = x_end - x_start;
    int dy = y_end - y_start;


    //check if slope is between 0 and 1
    if(abs(dx) >= abs(dy))
    {
        //slope is between 0 and 1
        if(dy >= 0)
        {
            // cout<<"dx:"<<dx<<" dy:"<<dy<<endl;
            //slope is between 0 and 1 and dx is positive
            int d = 2*dy - dx;
            int dE = 2*dy;
            int dNE = 2*(dy - dx);
            int x = x_start;
            int y = y_start;
            image.set_pixel(x, height-1-y, r, g, b);
            while(x < x_end)
            {
                
                if(d <= 0)
                {
                    d += dE;
                    x++;
                }
                else
                {
                    d += dNE;
                    x++;
                    y++;
                }

                image.set_pixel(x, height-1-y, r, g, b);
            }
            
        }
        else
        {
            //slope is between 0 and 1 and dx is negative
            //mirror the line with respect to y-axis

            dy = -dy;

            int d = 2*dy - dx;
            int dE = 2*dy;
            int dNE = 2*(dy - dx);
            int x = -x_start;
            int y = y_start;

            image.set_pixel(-x, height-1-y, r, g, b);
            while(-x < x_end)
            {
                
                if(d <= 0)
                {
                    d += dE;
                    x--;
                }
                else
                {
                    d += dNE;
                    x--;
                    y--;
                }

                image.set_pixel(-x, height-1-y, r, g, b);
            }

        }
    }
    else
    {
        // cout<<"here"<<endl;
        // cout<<"x1:"<<x_start<<" y1:"<<y_start<<" x2:"<<x_end<<" y2:"<<y_end<<endl;
        if(dy >= 0)
        {
            int d = 2*dx - dy;
            int dE = 2*dx;
            int dNE = 2*(dx - dy);

            int x = x_start;
            int y = y_start;

            image.set_pixel(x, height-1-y, r, g, b);
            while(y < y_end)
            {
                
                if(d <= 0)
                {
                    d += dE;
                    y++;
                }
                else
                {
                    d += dNE;
                    x++;
                    y++;
                }

                image.set_pixel(x, height-1-y, r, g, b);
            }
        }

        else
        {
            dy = -dy;

            int d = 2*dx - dy;
            int dE = 2*dx;
            int dNE = 2*(dx - dy);

            int x = -x_end;
            int y = y_end;

            // cout<<"x:"<<-x<<" y:"<<y<<endl;
            image.set_pixel(-x, height-1-y, r, g, b);
            while(y < y_start)
            {
                
                if(d <= 0)
                {
                    d += dE;
                    y++;
                }
                else
                {
                    d += dNE;
                    x++;
                    y++;
                }

                // cout<<"x:"<<-x<<" y:"<<y<<endl;
                image.set_pixel(-x, height-1-y, r, g, b);
            }
        }      
    }
}
double getCoverage(int x, int y, int x1, int y1, int dx, int dy)
{
    //divide a pixel into no_of_subpixels*no_of_subpixels grid
    int no_of_subpixels = 100;
    double y_north = y1 + 0.5;
    double y_south = y1 - 0.5;

    double x_new = x;
    double y_new = y;

    double sum = 0;
    double step = 1.0/no_of_subpixels;
    
    double intensity_step = 1.0/(no_of_subpixels*no_of_subpixels);

    for(int i=0; i<no_of_subpixels; i++)
    {
        for(int j=0; j< no_of_subpixels; j++)
        {
            x_new= x + step*(j+1);
            y_new = y + step*(i+1);

            //upper line
            double value_north = 2*(dy*x_new - dx*y_new + dx*y_north - dy*x1);
            //lower line
            double value_south = 2*(dy*x_new - dx*y_new + dx*y_south - dy*x1);

            //if subpixel lies inside the rectangle
            if(value_north >=0 && value_south <= 0)
            {
                sum += intensity_step;
            }
        }
    }

    cout<<"Pixel covered: "<<sum<<endl;

    return sum;


}


void mid_point_line_unweighted_area_sampling(int x_start, int y_start, int x_end, int y_end, int r, int g, int b, bitmap_image &image, int width, int height)
{   
     //(0,0) is bottom left corner
    //(W-1, H-1) is top right corner

    if(x_end < x_start)
    {
        swap(x_start, x_end);
        swap(y_start, y_end);
    }

    
    int dx = x_end - x_start;
    int dy = y_end - y_start;


    //check if slope is between 0 and 1
    if(abs(dx) >= abs(dy))
    {
        //slope is between 0 and 1
        if(dy >= 0)
        {
            // cout<<"dx:"<<dx<<" dy:"<<dy<<endl;
            //slope is between 0 and 1 and dx is positive
            int d = 2*dy - dx;
            int dE = 2*dy;
            int dNE = 2*(dy - dx);
            int x = x_start;
            int y = y_start;

            double intensity = getCoverage(x, y, x_end, y_end, dx, dy);
            image.set_pixel(x, height-1-y, round(r*intensity), round(g*intensity), round(b*intensity));
            intensity = getCoverage(x, y+1, x_end, y_end, dx, dy);
            image.set_pixel(x, height-1-(y+1), round(r*intensity), round(g*intensity), round(b*intensity));
            intensity = getCoverage(x, y-1, x_end, y_end, dx, dy);
            image.set_pixel(x, height-1-(y-1), round(r*intensity), round(g*intensity), round(b*intensity));

            while(x < x_end)
            {
                
                if(d <= 0)
                {
                    d += dE;
                    x++;
                }
                else
                {
                    d += dNE;
                    x++;
                    y++;
                }
                

                intensity = getCoverage(x, y, x_end, y_end, dx, dy);
                image.set_pixel(x, height-1-y, round(r*intensity), round(g*intensity), round(b*intensity));
                intensity = getCoverage(x, y+1, x_end, y_end, dx, dy);
                image.set_pixel(x, height-1-(y+1), round(r*intensity), round(g*intensity), round(b*intensity));
                intensity = getCoverage(x, y-1, x_end, y_end, dx, dy);
                image.set_pixel(x, height-1-(y-1), round(r*intensity), round(g*intensity), round(b*intensity));
            }
            
        }
        else
        {
            //slope is between 0 and 1 and dx is negative
            //mirror the line with respect to y-axis

            dy = -dy;

            int d = 2*dy - dx;
            int dE = 2*dy;
            int dNE = 2*(dy - dx);
            int x = -x_start;
            int y = y_start;

            // image.set_pixel(-x, height-1-y, r, g, b);
            double intensity = getCoverage(x, y, -x_end, y_end, dx, dy);
            image.set_pixel(-x, height-1-y, round(r*intensity), round(g*intensity), round(b*intensity));
            intensity = getCoverage(x, y+1, -x_end, y_end, dx, dy);
            image.set_pixel(-x, height-1-(y+1), round(r*intensity), round(g*intensity), round(b*intensity));
            intensity = getCoverage(x, y-1, -x_end, y_end, dx, dy);
            image.set_pixel(-x, height-1-(y-1), round(r*intensity), round(g*intensity), round(b*intensity));

            while(-x < x_end)
            {
                
                if(d <= 0)
                {
                    d += dE;
                    x--;
                }
                else
                {
                    d += dNE;
                    x--;
                    y--;
                }

                // image.set_pixel(-x, height-1-y, r, g, b);
                intensity = getCoverage(x, y, -x_end, y_end, dx, dy);
                image.set_pixel(-x, height-1-y, round(r*intensity), round(g*intensity), round(b*intensity));
                intensity = getCoverage(x, y+1, -x_end, y_end, dx, dy);
                image.set_pixel(-x, height-1-(y+1), round(r*intensity), round(g*intensity), round(b*intensity));
                intensity = getCoverage(x, y-1, -x_end, y_end, dx, dy);
                image.set_pixel(-x, height-1-(y-1), round(r*intensity), round(g*intensity), round(b*intensity));
            }

        }
    }
    else
    {
        // cout<<"here"<<endl;
        // cout<<"x1:"<<x_start<<" y1:"<<y_start<<" x2:"<<x_end<<" y2:"<<y_end<<endl;
        if(dy >= 0)
        {
            int d = 2*dx - dy;
            int dE = 2*dx;
            int dNE = 2*(dx - dy);

            int x = x_start;
            int y = y_start;

            //image.set_pixel(x, height-1-y, r, g, b);

            double intensity = getCoverage(x, y, x_end, y_end, dx, dy);
            image.set_pixel(x, height-1-y, round(r*intensity), round(g*intensity), round(b*intensity));
            intensity = getCoverage(x, y+1, x_end, y_end, dx, dy);
            image.set_pixel(x, height-1-(y+1), round(r*intensity), round(g*intensity), round(b*intensity));
            intensity = getCoverage(x, y-1, x_end, y_end, dx, dy);
            image.set_pixel(x, height-1-(y-1), round(r*intensity), round(g*intensity), round(b*intensity));
            
            while(y < y_end)
            {
                
                if(d <= 0)
                {
                    d += dE;
                    y++;
                }
                else
                {
                    d += dNE;
                    x++;
                    y++;
                }

                // image.set_pixel(x, height-1-y, r, g, b);
                intensity = getCoverage(x, y, x_end, y_end, dx, dy);
                image.set_pixel(x, height-1-y, round(r*intensity), round(g*intensity), round(b*intensity));
                intensity = getCoverage(x, y+1, x_end, y_end, dx, dy);
                image.set_pixel(x, height-1-(y+1), round(r*intensity), round(g*intensity), round(b*intensity));
                intensity = getCoverage(x, y-1, x_end, y_end, dx, dy);
                image.set_pixel(x, height-1-(y-1), round(r*intensity), round(g*intensity), round(b*intensity));
            }
        }

        else
        {
            dy = -dy;

            int d = 2*dx - dy;
            int dE = 2*dx;
            int dNE = 2*(dx - dy);

            int x = -x_end;
            int y = y_end;

            // cout<<"x:"<<-x<<" y:"<<y<<endl;
            // image.set_pixel(-x, height-1-y, r, g, b);
             double intensity = getCoverage(x, y, -x_end, y_end, dx, dy);
            image.set_pixel(-x, height-1-y, round(r*intensity), round(g*intensity), round(b*intensity));
            intensity = getCoverage(x, y+1, -x_end, y_end, dx, dy);
            image.set_pixel(-x, height-1-(y+1), round(r*intensity), round(g*intensity), round(b*intensity));
            intensity = getCoverage(x, y-1, -x_end, y_end, dx, dy);
            image.set_pixel(-x, height-1-(y-1), round(r*intensity), round(g*intensity), round(b*intensity));
            while(y < y_start)
            {
                
                if(d <= 0)
                {
                    d += dE;
                    y++;
                }
                else
                {
                    d += dNE;
                    x++;
                    y++;
                }

                // cout<<"x:"<<-x<<" y:"<<y<<endl;
                // image.set_pixel(-x, height-1-y, r, g, b);
                intensity = getCoverage(x, y, -x_end, y_end, dx, dy);
                image.set_pixel(-x, height-1-y, round(r*intensity), round(g*intensity), round(b*intensity));
                intensity = getCoverage(x, y+1, -x_end, y_end, dx, dy);
                image.set_pixel(-x, height-1-(y+1), round(r*intensity), round(g*intensity), round(b*intensity));
                intensity = getCoverage(x, y-1, -x_end, y_end, dx, dy);
                image.set_pixel(-x, height-1-(y-1), round(r*intensity), round(g*intensity), round(b*intensity));
            }
        }      
    }

   
   




   
}

int intensify_pixel(int x, int y, double d, int r, int g, int b, bitmap_image &image)
{
    cout<<"distance:"<<d<<endl; 

    d = abs(d);

    if(d > 1)
    {
       d = 1;
    }

    double intensity = 1-d;

    cout<<"intensity:"<<intensity<<endl;

    image.set_pixel(x, y, round(r*intensity), round(g*intensity), round(b*intensity));
}

void  mid_point_line_weighted_area_sampling(int x_start, int y_start, int x_end, int y_end, int r, int g, int b, bitmap_image &image, int width, int height)
{

    //(0,0) is bottom left corner
    //(W-1, H-1) is top right corner

    if(x_end < x_start)
    {
        swap(x_start, x_end);
        swap(y_start, y_end);
    }

    
    int dx = x_end - x_start;
    int dy = y_end - y_start;


    //check if slope is between 0 and 1
    if(abs(dx) >= abs(dy))
    {
        //slope is between 0 and 1
        if(dy >= 0)
        {
            // cout<<"dx:"<<dx<<" dy:"<<dy<<endl;
            //slope is between 0 and 1 and dx is positive
            int d = 2*dy - dx;
            int dE = 2*dy;
            int dNE = 2*(dy - dx);

            double two_v_dx = 0;
            double invDenom = 1.0/(2.0*sqrt(dx*dx + dy*dy));
            double two_dx_invDenom = 2.0*dx*invDenom;

            int x = x_start;
            int y = y_start;

            intensify_pixel(x, height-1-y, 0, r, g, b, image);
            intensify_pixel(x, height-1-(y+1), two_dx_invDenom, r, g, b, image);
            intensify_pixel(x, height-1-(y-1), two_dx_invDenom, r, g, b, image);
            // image.set_pixel(x, height-1-y, r, g, b);
            // image.set_pixel(x, height-1-(y+1), round(r*two_dx_invDenom), round(g*two_dx_invDenom), round(b*two_dx_invDenom));
            // image.set_pixel(x, height-1-(y-1), round(r*two_dx_invDenom), round(g*two_dx_invDenom), round(b*two_dx_invDenom));

            while(x < x_end)
            {
                
                if(d <= 0)
                {
                    two_v_dx += (d+dx);
                    d += dE;
                    x++;
                }
                else
                {
                    two_v_dx += (d-dx);
                    d += dNE;
                    x++;
                    y++;
                }

                intensify_pixel(x, height-1-y, two_v_dx*invDenom, r, g, b, image);
                intensify_pixel(x, height-1-(y+1), two_dx_invDenom - two_v_dx*invDenom, r, g, b, image);
                intensify_pixel(x, height-1-(y-1), two_dx_invDenom + two_v_dx*invDenom, r, g, b, image);

                // image.set_pixel(x, height-1-y, round(r*two_v_dx*invDenom), round(g*two_v_dx*invDenom), round(b*two_v_dx*invDenom));
                // image.set_pixel(x, height-1-(y+1), round(r*(two_dx_invDenom - two_v_dx*invDenom)), round(g*(two_dx_invDenom - two_v_dx*invDenom)), round(b*(two_dx_invDenom - two_v_dx*invDenom)));
                // image.set_pixel(x, height-1-(y-1), round(r*(two_dx_invDenom + two_v_dx*invDenom)), round(g*(two_dx_invDenom + two_v_dx*invDenom)), round(b*(two_dx_invDenom +two_v_dx*invDenom)));
            }
            
        }
        else
        {
            //slope is between 0 and 1 and dx is negative
            //mirror the line with respect to y-axis

            dy = -dy;

            int d = 2*dy - dx;
            int dE = 2*dy;
            int dNE = 2*(dy - dx);

            double two_v_dx = 0;
            double invDenom = 1.0/(2.0*sqrt(dx*dx + dy*dy));
            double two_dx_invDenom = 2.0*dx*invDenom;

            int x = -x_start;
            int y = y_start;
            

            
            intensify_pixel(-x, height-1-y, 0, r, g, b, image);
            intensify_pixel(-x, height-1-(y+1), two_dx_invDenom, r, g, b, image);
            intensify_pixel(-x, height-1-(y-1), two_dx_invDenom, r, g, b, image);

            // image.set_pixel(-x, height-1-y, r, g, b);
            // image.set_pixel(-x, height-1-(y+1), round(r*two_dx_invDenom), round(g*two_dx_invDenom), round(b*two_dx_invDenom));
            // image.set_pixel(-x, height-1-(y-1), round(r*two_dx_invDenom), round(g*two_dx_invDenom), round(b*two_dx_invDenom));

            //image.set_pixel(-x, height-1-y, r, g, b);
            while(-x < x_end)
            {
                
                if(d <= 0)
                {
                    two_v_dx += (d+dx);
                    d += dE;
                    x--;
                }
                else
                {
                    two_v_dx += (d-dx);
                    d += dNE;
                    x--;
                    y--;
                }

                //image.set_pixel(-x, height-1-y, r, g, b);

                // image.set_pixel(-x, height-1-y, round(r*two_v_dx*invDenom), round(g*two_v_dx*invDenom), round(b*two_v_dx*invDenom));
                // image.set_pixel(-x, height-1-(y+1), round(r*(two_dx_invDenom - two_v_dx*invDenom)), round(g*(two_dx_invDenom - two_v_dx*invDenom)), round(b*(two_dx_invDenom - two_v_dx*invDenom)));
                // image.set_pixel(-x, height-1-(y-1), round(r*(two_dx_invDenom + two_v_dx*invDenom)), round(g*(two_dx_invDenom + two_v_dx*invDenom)), round(b*(two_dx_invDenom +two_v_dx*invDenom)));

                intensify_pixel(-x, height-1-y, two_v_dx*invDenom, r, g, b, image);
                intensify_pixel(-x, height-1-(y+1), two_dx_invDenom - two_v_dx*invDenom, r, g, b, image);
                intensify_pixel(-x, height-1-(y-1), two_dx_invDenom + two_v_dx*invDenom, r, g, b, image);

            }

        }
    }
    else
    {
        // cout<<"here"<<endl;
        // cout<<"x1:"<<x_start<<" y1:"<<y_start<<" x2:"<<x_end<<" y2:"<<y_end<<endl;
        if(dy >= 0)
        {
            int d = 2*dx - dy;
            int dE = 2*dx;
            int dNE = 2*(dx - dy);

            double two_v_dy = 0;
            double invDenom = 1.0/(2.0*sqrt(dx*dx + dy*dy));
            double two_dy_invDenom = 2.0*dy*invDenom;

            int x = x_start;
            int y = y_start;

            //image.set_pixel(x, height-1-y, r, g, b);
            intensify_pixel(x, height-1-y, 0, r, g, b, image);
            intensify_pixel(x, height-1-(y+1), two_dy_invDenom, r, g, b, image);
            intensify_pixel(x, height-1-(y-1), two_dy_invDenom, r, g, b, image);


            while(y < y_end)
            {
                
                if(d <= 0)
                {
                    two_v_dy += (d+dy);
                    d += dE;
                    y++;
                }
                else
                {
                    two_v_dy += (d-dy);
                    d += dNE;
                    x++;
                    y++;
                }

                //image.set_pixel(x, height-1-y, r, g, b);
                intensify_pixel(x, height-1-y, two_v_dy*invDenom, r, g, b, image);
                intensify_pixel(x, height-1-(y+1), two_dy_invDenom - two_v_dy*invDenom, r, g, b, image);
                intensify_pixel(x, height-1-(y-1), two_dy_invDenom + two_v_dy*invDenom, r, g, b, image);
            }
        }

        else
        {
            dy = -dy;

            int d = 2*dx - dy;
            int dE = 2*dx;
            int dNE = 2*(dx - dy);

            double two_v_dy = 0;
            double invDenom = 1.0/(2.0*sqrt(dx*dx + dy*dy));
            double two_dy_invDenom = 2.0*dy*invDenom;

            int x = -x_end;
            int y = y_end;

            // cout<<"x:"<<-x<<" y:"<<y<<endl;
            //image.set_pixel(-x, height-1-y, r, g, b);
            intensify_pixel(-x, height-1-y, 0, r, g, b, image);
            intensify_pixel(-x, height-1-(y+1), two_dy_invDenom, r, g, b, image);
            intensify_pixel(-x, height-1-(y-1), two_dy_invDenom, r, g, b, image);
            while(y < y_start)
            {
                
                if(d <= 0)
                {
                    two_v_dy += (d+dy);
                    d += dE;
                    y++;
                }
                else
                {
                    two_v_dy += (d-dy);
                    d += dNE;
                    x++;
                    y++;
                }

                // cout<<"x:"<<-x<<" y:"<<y<<endl;
                //image.set_pixel(-x, height-1-y, r, g, b);
                intensify_pixel(-x, height-1-y, two_v_dy*invDenom, r, g, b, image);
                intensify_pixel(-x, height-1-(y+1), two_dy_invDenom - two_v_dy*invDenom, r, g, b, image);
                intensify_pixel(-x, height-1-(y-1), two_dy_invDenom + two_v_dy*invDenom, r, g, b, image);
            }
        }      
    }

    
    
}

int main()
{
    //read from file
    ifstream in("input.txt");

    //dimensions of bitmap image
    int width, height;
    in >> width >> height;

    //number of lines to draw
    int N;
    in >> N;

    //create bitmap image
    bitmap_image image_1(width, height), image_2(width, height), image_3(width, height);

    //set background color
    image_1.set_all_channels(0,0,0);
    image_2.set_all_channels(0,0,0);
    image_3.set_all_channels(0,0,0);

    for(int i=0; i<N; i++)
    {
        //read line coordinates
        int x_start, y_start, x_end, y_end;
        in >> x_start >> y_start >> x_end >> y_end;

        //get color
        int r, g, b;
        in >> r >> g >> b;

        

        //draw line with midpoint algorithm no antialiasing
        mid_point_line(x_start,y_start, x_end, y_end, r, g, b, image_1, width, height);

        //draw line with midpoint algorithm with unweighted area sampling
        mid_point_line_unweighted_area_sampling(x_start,y_start, x_end, y_end, r, g, b, image_2, width, height);

        //draw line with midpoint algorithm with weighted area sampling (Gupta-Sproull filter)
        mid_point_line_weighted_area_sampling(x_start,y_start, x_end, y_end, r, g, b, image_3, width, height);
        
        
    }
    image_1.save_image("1_R.bmp");
    image_2.save_image("2_RUA.bmp");
    image_3.save_image("3_RWA.bmp");

}