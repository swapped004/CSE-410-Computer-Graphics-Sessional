double getAreaCovered(int dx, int dy, int x, int y, int x1, int y1)
{
    int subPixel = 100;
    double y_upper = y1 + 0.5;
    double y_lower = y1 - 0.5;

    double x_temp = x;
    double y_temp = y;
    double covered = 0.0;
    double step = 1.0 / subPixel;
    double intensity_increase = 1.0 / (subPixel * subPixel);
    for (int i = 0; i < subPixel; i++)
    {
        for (int j = 0; j < subPixel; j++)
        {
            x_temp = x * 1.0 + step * (j + 1);
            y_temp = y * 1.0 + step * (i + 1);

            // getting d = 2*(ax+by+c) = 2*(dy*x - dx*y + dx*y1 - dy*x1) value:

            double upper_func_value = 2 * (dy * x_temp - dx * y_temp + dx * y_upper - dy * x1);
            double lower_func_value = 2 * (dy * x_temp - dx * y_temp + dx * y_lower - dy * x1);

            if (upper_func_value >= 0 && lower_func_value <= 0)
            // if (upper_func_value <= 0 && lower_func_value >= 0)

            {
                covered += intensity_increase;
            }
        }
    }

    cout << "Inside getAreaCovered: " << covered << endl;

    return covered;
}

void antiAliasingUnweighted(){
    // anti-aliasing algorithm
    for(int i = 0; i < w; i++){
        for(int j = 0; j < h; j++){
            image.set_pixel(i, j, 255, 255, 255);
        }
    }


    for (int i = 0; i < n; i++)
    {
        int x1 = allLines[i].x1;
        int y1 = allLines[i].y1;
        int x2 = allLines[i].x2;
        int y2 = allLines[i].y2;
        int dx = x2 - x1;
        int dy = y2 - y1;
        int d = 2 * dy - dx;
        int dE = 2 * dy;
        int dNE = 2 * (dy - dx);
        int x = x1;
        int y = y1;

        double tmpIntensity = 1-getAreaCovered(dx, dy, x, y, x2, y2);

        image.set_pixel(x, h-1-y, 255*tmpIntensity, 255*tmpIntensity, 255*tmpIntensity);
        tmpIntensity = 1-getAreaCovered(dx, dy, x, y+1, x2, y2);
        image.set_pixel(x, h-1-y-1, 255*tmpIntensity, 255*tmpIntensity, 255*tmpIntensity);
        tmpIntensity = 1-getAreaCovered(dx, dy, x, y-1, x2, y2);
        image.set_pixel(x, h-1-y+1, 255*tmpIntensity, 255*tmpIntensity, 255*tmpIntensity);


        while (x < x2)
        {
            if (d <= 0)
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
            tmpIntensity = 1-getAreaCovered(dx, dy, x, y, x2, y2);
            image.set_pixel(x, h-1-y, 255*tmpIntensity, 255*tmpIntensity, 255*tmpIntensity);
            tmpIntensity = 1-getAreaCovered(dx, dy, x, y+1, x2, y2);
            image.set_pixel(x, h-1-y-1, 255*tmpIntensity, 255*tmpIntensity, 255*tmpIntensity);
            tmpIntensity = 1-getAreaCovered(dx, dy, x, y-1, x2, y2);
            image.set_pixel(x, h-1-y+1, 255*tmpIntensity, 255*tmpIntensity, 255*tmpIntensity);
        }
    }
    image.save_image("2_RUA.bmp");
}