#include <iostream>
#include <opencv2/opencv.hpp>  
#include "opencv2/imgproc/imgproc_c.h"
#include<highgui.hpp>
#include<opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <windows.h>  
#include <stdio.h>
#include <string>
using namespace cv;
using namespace std;


int C_NUM = 0;//bit
void show_info(Mat img)
{
    cout << "img.dims  = " << img.dims << endl;
    cout << "img.rows  = " << img.rows << endl;
    cout << "img.cols  = " << img.cols << endl;
    cout << "img.channels = " << img.channels() << endl;
    cout << "img.step[0] = " << img.step[0] << endl;
    cout << "img.step[1] = " << img.step[1] << endl;
    cout << "img.Elemsize = " << img.elemSize() << endl;    //一个像素点的大小  CV_8U3C=1*3
    cout << "img.Elemsize1 = " << img.elemSize1() << endl;  //数据类型的大小 UCHAR = 1 （ElemSize / Channel）
}
void show_pix(Mat img)
{
    int cnt = 0;
    for (int i = 0; i < img.rows; i++)
    {
        for (int j = 0; j < img.cols; j++)
        {
            Vec3b bgr = img.at<Vec3b>(i, j);
            cout << "pix " << cnt << ":";
            cout << "B: " << (unsigned int)bgr.val[0] << ", ";
            cout << "G: " << (unsigned int)bgr.val[1] << ", ";
            cout << "R: " << (unsigned int)bgr.val[2] << endl;
            cnt++;
        }
    }
}

bool get_char_bit(char c, int i)
{
    return ((c >> i) & 1);
}

void change_img(Mat img, bool x)
{

    int word_index = C_NUM / 9;//第几个字节
    int pix_index = C_NUM / 3;//第几个像素
    int row = pix_index / img.cols;
    int col = pix_index % img.cols;

    int color_index_word = C_NUM % 9;//一个字节内第几个颜色分量
    int color_index_pix = C_NUM % 3;//一个像素内第几个颜色分量

    Vec3b pixel = img.at<Vec3b>(row, col);//一个像素
    unsigned int a = pixel[color_index_pix];

    //cout << a<<"&"<<x;

    if (a % 2)//奇数
    {
        if (!x)
        {
            a &= ~1;
        }
    }
    else if (x)
    {
        a += 1;
    }
    //cout << " = "<<a << endl;

    pixel[color_index_pix] = (unsigned char)a;


    Vec3b changed;
    changed[0] = pixel.val[0];  //Blue
    changed[1] = pixel.val[1];  //Green
    changed[2] = pixel.val[2];  //Red
    img.at<Vec3b>(row, col) = changed;

   


    if (color_index_word == 7)
    {
        C_NUM ++;
        change_img(img,0);//第三个像素的最后一个R里面的最后一个信息位不用，因为前面已经八个表示一个字节了，
    } 
    else
        C_NUM++;
}

bool enchange_img(Mat img)
{
    int word_index = C_NUM / 9;//第几个字节
    int pix_index = C_NUM / 3;//第几个像素
    int row = pix_index / img.cols;
    int col = pix_index % img.cols;

    int color_index_word = C_NUM % 9;//一个字节内第几个颜色分量
    int color_index_pix = C_NUM % 3;//一个像素内第几个颜色分量

    Vec3b pixel = img.at<Vec3b>(row, col);//一个像素
    unsigned int a = pixel[color_index_pix];

    if (color_index_word == 7)
        C_NUM+=2;
    else
        C_NUM++;


    
    return (a & 1);

    
}

void encrypt_img(Mat img, string s)
{
    const char* p = s.data();
    int len = s.length();
    char c;
    for (int j = 0; j < len; j++)
    {
        c = s[j];
        for (int i = 0; i < 8; i++)
        {
            bool x = (c >> (7 - i)) & 1;
            change_img(img, x);
        }
    }
    c = 27;//结尾符号
    for (int i = 0; i < 8; i++)
    {
        bool x = (c >> (7 - i)) & 1;
        change_img(img, x);
    }
}

void decrypt_img(Mat img, char* s)
{
    C_NUM = 0;
    int len = (img.rows * img.cols) / 9;
    char c;
    for (int index = 0; index < len; index++)
    {
        unsigned int a = 0;
        bool x;
        for (int i = 7; i >= 0 ; i--)
        {
            if (enchange_img(img))
            {
                a += pow(2, i);
            }
        }
        if (a == 27)
        {
            s[index] = '\0';
            break;
        }

        char c = a;

        s[index] = c;
    }
}

void encrypt(string fp)
{
   Mat img = imread(fp);
   if (!img.data)
   {
       cout << "读取图片错误！！！" << endl;
       return;
   }

   cout << "输入你的话"<<endl;

   string word;
   getline(cin, word);
   int len = word.length();

   while (len > ((img.rows * img.cols) / 3))
   {
       word.clear();

       cout << "你话太多了!!!!" << endl;
       getline(cin, word);
       len = word.length();
   }
  
   encrypt_img(img, word);

   vector<int> compression_params;
   compression_params.push_back(IMWRITE_PNG_COMPRESSION);
   compression_params.push_back(0);    // 无压缩png.
   compression_params.push_back(IMWRITE_PNG_STRATEGY);
   compression_params.push_back(IMWRITE_PNG_STRATEGY_DEFAULT);

   imwrite("end.png", img, compression_params);//必须使用png格式才能无压缩的存储图片

   cout << "加密完成。查看end.png"<<endl;
}

void decrypt(string fp)
{
    Mat img = imread(fp);
    if (!img.data)
    {
        cout << "读取图片错误！！！" << endl;
        return;
    }
    char s_out[2000] = "\0";
    decrypt_img(img, s_out);
    cout << "解密完成：\n" << endl;
    cout << s_out<<endl << endl;
}

int main() {


    string command;
    string file_path;

    cout << "使用/decrypt 1.png 命令解密文件" << endl;
    cout << "使用/encrypt 1.png/jpg 命令加密文件" << endl;
    cout << "使用/quit退出程序" << endl;
    getline(cin, command);
    while (command != "/quit")
    {
        int c1 = command.find("/decrypt ");
        int c2 = command.find("/encrypt ");
        int c3 = command.find("/quit");

        if (c1 != command.npos)
        {
            file_path = command.substr(9);
            decrypt(file_path);

        }
        else if (c2 != command.npos)
        {
            file_path = command.substr(9);
            encrypt(file_path);

        }
        else if (c3 != command.npos)
        {
            return 0;
        }
        system("pause");
        system("cls");
        cout << "使用/decrypt 1.png 命令解压文件" << endl;
        cout << "使用/encrypt 1.png/jpg 命令压缩文件" << endl;
        cout << "使用/quit退出程序" << endl;
        getline(cin, command);
    }

    //char s_in[] = "《保安日记》\n\n 11月2日 晴 大风\n\n  多希望你对我说\n\n    “宝，早安”\n\n  而不是\n\n    “早，保安”\n\n";


    waitKey(0);    //在键盘敲入字符前程序处于等待状态
    destroyAllWindows();    //关闭所有窗口
    return 0;
}