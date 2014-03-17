#include <cv.h>
#include <highgui.h>

#include <stdio.h>
#include <iostream>

// for directory and file reading
#include <unistd.h>
#include <dirent.h>

int main(int argc, char** argv)
{

    if (argc == 1)
    {
        std::cout << "./images_merger <DIR> <key_delay>" << std::endl;
        return 0;
    }
    
    std::string Data_DIR = argv[1]; // "../0315_roof/";

    int key_delay = 30;
    
    if (argc == 3)
        key_delay = atoi(argv[2]);
    
//  std::cout << key_delay << std::endl;

    std::string LDIR = Data_DIR + "left";
    std::string RDIR = Data_DIR + "right";
    std::string DDIR = Data_DIR + "disp";

    int trj_size = 0;
    struct dirent *pDirent;
    DIR *pDir;

    pDir = opendir(LDIR.c_str());
    if (pDir != NULL)
    {
        while ((pDirent = readdir(pDir)) != NULL)
        {
            if (!strcmp(pDirent->d_name, ".") || !strcmp(pDirent->d_name, ".."))
                continue;    /* skip self and parent */
            
            trj_size++;
        }
    }
    closedir (pDir);

    cv::Mat imgl;
    cv::Mat imgr;
    cv::Mat imgd;
    
    std::string L_FILE; // left file name
    std::string R_FILE; // right file name
    std::string D_FILE; // disparity file name
    
    for (int index = 0; index < trj_size; index ++)
    {
        
        std::stringstream s_prefix; // tmp string to hold prefix
        s_prefix << std::setfill('0') << std::setw(5) << index;

        L_FILE = LDIR + "/im" + s_prefix.str() + "_imgl.bmp";
        R_FILE = RDIR + "/im" + s_prefix.str() + "_imgr.bmp";
        D_FILE = DDIR + "/im" + s_prefix.str() + "_disp.bmp";

        imgl = cv::imread(L_FILE, CV_LOAD_IMAGE_GRAYSCALE);
        if (imgl.empty())
        {
            std::cout << "Fail to load left image " << index << std::endl;
            return 0;
        }

        imgr = cv::imread(R_FILE, CV_LOAD_IMAGE_GRAYSCALE);
        if (imgr.empty())
        {
            std::cout << "Fail to load right image " << index << std::endl;
            return 0;
        }

        imgd = cv::imread(D_FILE, CV_LOAD_IMAGE_GRAYSCALE);
        if (imgd.empty())
        {
            std::cout << "Fail to load disparity image " << index << std::endl;
            return 0;
        }

        // StereoBM
        cv::Mat disp, disp8;    
    
        cv::StereoBM sbm;
        sbm.state->SADWindowSize = 9;
        sbm.state->numberOfDisparities = 64;
        sbm.state->preFilterSize = 9;
        sbm.state->preFilterCap = 31;
        sbm.state->minDisparity = 0;
        sbm.state->textureThreshold = 10;
        sbm.state->uniquenessRatio = 30;
        sbm.state->speckleWindowSize = 100;
        sbm.state->speckleRange = 2;
        sbm.state->disp12MaxDiff = -1;
    
        sbm(imgl, imgr, disp);
        cv::normalize(disp, disp8, 0, 255, CV_MINMAX, CV_8U);

        int width;
        int height;
        
        height = imgl.rows;
        width = imgl.cols;

        cv::Mat m_img(3*height+20, 2*width+10, CV_8UC1, 255); // merged image
                
        cv::Rect roi = cv::Rect(0, 0, width, height);
        imgl.copyTo(m_img(roi));
        
        roi = cv::Rect(0, height+10, width, height);
        imgd.copyTo(m_img(roi));

        roi = cv::Rect(0, 2*height+20, width, height);
        imgr.copyTo(m_img(roi));
        
        roi = cv::Rect(width+10, height+10, width, height);
        disp8.copyTo(m_img(roi));
        

#if 0
        for (int i = 5; i < width; i+=20)
            cv::line(m_img, cv::Point(i, 0), cv::Point(i, 3*height+20), CV_RGB(0, 0, 0), 0.1, 4); // not able to print color since the image is gray scale
#endif

        cv::imshow("Left|Disparity|Right Image", m_img);
        cv::moveWindow("Left|Disparity|Right Image", 100, 100);
        
         switch (cv::waitKey(key_delay)) // give a positive number to make it auto
        {
        case 27: // capture 'Esc'
            return 0;
        case 'q':
            return 0;
        case 'k':
            index-=2;
        }
        
    }

    cv::destroyAllWindows();    
    return 1;
    
}
