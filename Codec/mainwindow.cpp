#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <stdio.h>
#include <QtCore/qmath.h>
#include <QDebug>
#include <QtNetwork>

#include <QByteArray>
#include <QCoreApplication>
#include <QHostAddress>
#include <QUdpSocket>

#include <stdint.h>

const quint16 PORT = 2333;

#define F_PATH "d:\\myfile\\file.dat"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init_show();
}

MainWindow::~MainWindow()
{
    delete ui;
}

#if 1

/**
 * Split Y, U, V planes in YUV420P file.
 * @param url  Location of Input YUV file.
 * @param w    Width of Input YUV file.
 * @param h    Height of Input YUV file.
 * @param num  Number of frames to process.
 *
 */
int simplest_yuv420_split(char *url, int w, int h,int num)
{
        FILE *fp=fopen(url,"rb+");
        FILE *fp1=fopen("output_420_y.y","wb+");
        FILE *fp2=fopen("output_420_u.y","wb+");
        FILE *fp3=fopen("output_420_v.y","wb+");

        unsigned char *pic=(unsigned char *)malloc(w*h*3/2);

        for(int i=0;i<num;i++){

                fread(pic,1,w*h*3/2,fp);
                //Y
                fwrite(pic,1,w*h,fp1);
                //U
                fwrite(pic+w*h,1,w*h/4,fp2);
                //V
                fwrite(pic+w*h*5/4,1,w*h/4,fp3);
        }

        free(pic);
        fclose(fp);
        fclose(fp1);
        fclose(fp2);
        fclose(fp3);

        return 0;
}

/**
 * Split Y, U, V planes in YUV444P file.
 * @param url  Location of YUV file.
 * @param w    Width of Input YUV file.
 * @param h    Height of Input YUV file.
 * @param num  Number of frames to process.
 *
 */
int simplest_yuv444_split(char *url, int w, int h,int num){
        FILE *fp=fopen(url,"rb+");
        FILE *fp1=fopen("output_444_y.y","wb+");
        FILE *fp2=fopen("output_444_u.y","wb+");
        FILE *fp3=fopen("output_444_v.y","wb+");
        unsigned char *pic=(unsigned char *)malloc(w*h*3);

        for(int i=0;i<num;i++){
                fread(pic,1,w*h*3,fp);
                //Y
                fwrite(pic,1,w*h,fp1);
                //U
                fwrite(pic+w*h,1,w*h,fp2);
                //V
                fwrite(pic+w*h*2,1,w*h,fp3);
        }

        free(pic);
        fclose(fp);
        fclose(fp1);
        fclose(fp2);
        fclose(fp3);

        return 0;
}

/**
 * Convert YUV420P file to gray picture
 * @param url     Location of Input YUV file.
 * @param w       Width of Input YUV file.
 * @param h       Height of Input YUV file.
 * @param num     Number of frames to process.
 */
int simplest_yuv420_gray(char *url, int w, int h,int num){
        FILE *fp=fopen(url,"rb+");
        FILE *fp1=fopen("output_gray.yuv","wb+");
        unsigned char *pic=(unsigned char *)malloc(w*h*3/2);

        for(int i=0;i<num;i++){
                fread(pic,1,w*h*3/2,fp);
                //Gray
                memset(pic+w*h,128,w*h/2);
                fwrite(pic,1,w*h*3/2,fp1);
        }

        free(pic);
        fclose(fp);
        fclose(fp1);
        return 0;
}
#endif

void MainWindow::on_pushButton_clicked()
{
    simplest_yuv420_split("D:\\liuqs_myself\\Qt-pro\\untitled6\\lena_256x256_yuv420p.yuv",256,256,1);
     ui->pushButton->setText("yuv420_split ok");

}

void MainWindow::on_pushButton_2_clicked()
{
     simplest_yuv420_gray("D:\\liuqs_myself\\Qt-pro\\untitled6\\lena_256x256_yuv420p.yuv",256,256,1);
     ui->pushButton_2->setText("yuv420_gray ok");
}

void MainWindow::on_pushButton_3_clicked()
{
    simplest_yuv444_split("D:\\liuqs_myself\\Qt-pro\\untitled6\\lena_256x256_yuv444p.yuv",256,256,1);
    ui->pushButton_3->setText("yuv444_split ok");
}

/**
 * Add border for YUV420P file
 * @param url     Location of Input YUV file.
 * @param w       Width of Input YUV file.
 * @param h       Height of Input YUV file.
 * @param border  Width of Border.
 * @param num     Number of frames to process.
 */
int simplest_yuv420_border(char *url, int w, int h,int border,int num){
        FILE *fp=fopen(url,"rb+");
        FILE *fp1=fopen("output_border.yuv","wb+");

        unsigned char *pic=(unsigned char *)malloc(w*h*3/2);

        for(int i=0;i<num;i++){
                fread(pic,1,w*h*3/2,fp);
                //Y
                for(int j=0;j<h;j++){
                        for(int k=0;k<w;k++){
                                if(k<border||k>(w-border)||j<border||j>(h-border)){
                                        pic[j*w+k]=255;
                                        //pic[j*w+k]=0;
                                }
                        }
                }
                fwrite(pic,1,w*h*3/2,fp1);
        }

        free(pic);
        fclose(fp);
        fclose(fp1);

        return 0;
}

void MainWindow::on_pushButton_4_clicked()
{
    simplest_yuv420_border("D:\\liuqs_myself\\Qt-pro\\untitled6\\lena_256x256_yuv420p.yuv",256,256,20,1);
    ui->pushButton_4->setText("yuv420_border ok");
}

/**
 * Generate YUV420P gray scale bar.
 * @param width    Width of Output YUV file.
 * @param height   Height of Output YUV file.
 * @param ymin     Max value of Y
 * @param ymax     Min value of Y
 * @param barnum   Number of bars
 * @param url_out  Location of Output YUV file.
 */
int simplest_yuv420_graybar(int width, int height,int ymin,int ymax,int barnum,char *url_out){
        int barwidth;
        float lum_inc;
        unsigned char lum_temp;
        int uv_width,uv_height;
        FILE *fp=NULL;
        unsigned char *data_y=NULL;
        unsigned char *data_u=NULL;
        unsigned char *data_v=NULL;
        int t=0,i=0,j=0;

        barwidth=width/barnum;
        lum_inc=((float)(ymax-ymin))/((float)(barnum-1));
        uv_width=width/2;
        uv_height=height/2;

        data_y=(unsigned char *)malloc(width*height);
        data_u=(unsigned char *)malloc(uv_width*uv_height);
        data_v=(unsigned char *)malloc(uv_width*uv_height);

        if((fp=fopen(url_out,"wb+"))==NULL){
                printf("Error: Cannot create file!");
                return -1;
        }

        //Output Info
        printf("Y, U, V value from picture's left to right:\n");
        for(t=0;t<(width/barwidth);t++){
                lum_temp=ymin+(char)(t*lum_inc);
                printf("%3d, 128, 128\n",lum_temp);
        }
        //Gen Data
        for(j=0;j<height;j++){
                for(i=0;i<width;i++){
                        t=i/barwidth;
                        lum_temp=ymin+(char)(t*lum_inc);
                        data_y[j*width+i]=lum_temp;
                }
        }
        for(j=0;j<uv_height;j++){
                for(i=0;i<uv_width;i++){
                        data_u[j*uv_width+i]=128;
                }
        }
        for(j=0;j<uv_height;j++){
                for(i=0;i<uv_width;i++){
                        data_v[j*uv_width+i]=128;
                }
        }
        fwrite(data_y,width*height,1,fp);
        fwrite(data_u,uv_width*uv_height,1,fp);
        fwrite(data_v,uv_width*uv_height,1,fp);
        fclose(fp);
        free(data_y);
        free(data_u);
        free(data_v);
        return 0;
}
//simplest_yuv420_graybar(640, 360,0,255,10,"D:\\liuqs_myself\\Qt-pro\\untitled6\\graybar_640x360.yuv");
//ui->pushButton_5->setText("yuv420_border ok");


void MainWindow::on_pushButton_5_clicked()
{
    simplest_yuv420_graybar(640, 360,0,255,10,"D:\\liuqs_myself\\Qt-pro\\untitled6\\graybar_640x360.yuv");
    ui->pushButton_5->setText("yuv420_graybar ok");
}

/**
 * Calculate PSNR between 2 YUV420P file
 * @param url1     Location of first Input YUV file.
 * @param url2     Location of another Input YUV file.
 * @param w        Width of Input YUV file.
 * @param h        Height of Input YUV file.
 * @param num      Number of frames to process.
 */
int simplest_yuv420_psnr(char *url1,char *url2,int w,int h,int num)
{
        int ret = 0;
        FILE *fp1=fopen(url1,"rb+");
        FILE *fp2=fopen(url2,"rb+");
        unsigned char *pic1=(unsigned char *)malloc(w*h);
        unsigned char *pic2=(unsigned char *)malloc(w*h);

        for(int i=0;i<num;i++){
                fread(pic1,1,w*h,fp1);
                fread(pic2,1,w*h,fp2);

                double mse_sum=0,mse=0,psnr=0;
                for(int j=0;j<w*h;j++){
                        mse_sum+=pow((double)(pic1[j]-pic2[j]),2);
                }
                mse=mse_sum/(w*h);
                psnr=10*log10(255.0*255.0/mse);
                printf("%5.3f\n",psnr);
                qDebug() << "%5.3f" << psnr<<endl;
                ret = psnr;

                fseek(fp1,w*h/2,SEEK_CUR);
                fseek(fp2,w*h/2,SEEK_CUR);
        }

        free(pic1);
        free(pic2);
        fclose(fp1);
        fclose(fp2);

        return ret;
}

void MainWindow::on_pushButton_6_clicked()
{
    int num = simplest_yuv420_psnr("D:\\liuqs_myself\\Qt-pro\\untitled6\\lena_256x256_yuv420p.yuv","D:\\liuqs_myself\\Qt-pro\\untitled6\\lena_distort_256x256_yuv420p.yuv",256,256,1);
    ui->pushButton_6->setText(QString::number(num));
}

/**
 * Split R, G, B planes in RGB24 file.
 * @param url  Location of Input RGB file.
 * @param w    Width of Input RGB file.
 * @param h    Height of Input RGB file.
 * @param num  Number of frames to process.
 *
 */
int simplest_rgb24_split(char *url, int w, int h,int num){
        FILE *fp=fopen(url,"rb+");
        FILE *fp1=fopen("output_r.y","wb+");
        FILE *fp2=fopen("output_g.y","wb+");
        FILE *fp3=fopen("output_b.y","wb+");

        unsigned char *pic=(unsigned char *)malloc(w*h*3);

        for(int i=0;i<num;i++){

                fread(pic,1,w*h*3,fp);

                for(int j=0;j<w*h*3;j=j+3){
                        //R
                        fwrite(pic+j,1,1,fp1);
                        //G
                        fwrite(pic+j+1,1,1,fp2);
                        //B
                        fwrite(pic+j+2,1,1,fp3);
                }
        }

        free(pic);
        fclose(fp);
        fclose(fp1);
        fclose(fp2);
        fclose(fp3);

        return 0;
}

void MainWindow::on_pushButton_7_clicked()
{
    simplest_rgb24_split("D:\\liuqs_myself\\Qt-pro\\untitled6\\cie1931_500x500.rgb", 500, 500,1);
    ui->pushButton_7->setText("rgb24_split ok");
}

/**
 * Convert RGB24 file to BMP file
 * @param rgb24path    Location of input RGB file.
 * @param width        Width of input RGB file.
 * @param height       Height of input RGB file.
 * @param url_out      Location of Output BMP file.
 */
int simplest_rgb24_to_bmp(const char *rgb24path,int width,int height,const char *bmppath){
        typedef struct
        {
                long imageSize;
                long blank;
                long startPosition;
        }BmpHead;

        typedef struct
        {
                long  Length;
                long  width;
                long  height;
                unsigned short  colorPlane;
                unsigned short  bitColor;
                long  zipFormat;
                long  realSize;
                long  xPels;
                long  yPels;
                long  colorUse;
                long  colorImportant;
        }InfoHead;

        int i=0,j=0;
        BmpHead m_BMPHeader={0};
        InfoHead  m_BMPInfoHeader={0};
        char bfType[2]={'B','M'};
        int header_size=sizeof(bfType)+sizeof(BmpHead)+sizeof(InfoHead);
        unsigned char *rgb24_buffer=NULL;
        FILE *fp_rgb24=NULL,*fp_bmp=NULL;

        if((fp_rgb24=fopen(rgb24path,"rb"))==NULL){
                printf("Error: Cannot open input RGB24 file.\n");
                return -1;
        }
        if((fp_bmp=fopen(bmppath,"wb"))==NULL){
                printf("Error: Cannot open output BMP file.\n");
                return -1;
        }

        rgb24_buffer=(unsigned char *)malloc(width*height*3);
        fread(rgb24_buffer,1,width*height*3,fp_rgb24);

        m_BMPHeader.imageSize=3*width*height+header_size;
        m_BMPHeader.startPosition=header_size;

        m_BMPInfoHeader.Length=sizeof(InfoHead);
        m_BMPInfoHeader.width=width;
        //BMP storage pixel data in opposite direction of Y-axis (from bottom to top).
        m_BMPInfoHeader.height=-height;
        m_BMPInfoHeader.colorPlane=1;
        m_BMPInfoHeader.bitColor=24;
        m_BMPInfoHeader.realSize=3*width*height;

        fwrite(bfType,1,sizeof(bfType),fp_bmp);
        fwrite(&m_BMPHeader,1,sizeof(m_BMPHeader),fp_bmp);
        fwrite(&m_BMPInfoHeader,1,sizeof(m_BMPInfoHeader),fp_bmp);

        //BMP save R1|G1|B1,R2|G2|B2 as B1|G1|R1,B2|G2|R2
        //It saves pixel data in Little Endian
        //So we change 'R' and 'B'
        for(j =0;j<height;j++){
                for(i=0;i<width;i++){
                        char temp=rgb24_buffer[(j*width+i)*3+2];
                        rgb24_buffer[(j*width+i)*3+2]=rgb24_buffer[(j*width+i)*3+0];
                        rgb24_buffer[(j*width+i)*3+0]=temp;
                }
        }
        fwrite(rgb24_buffer,3*width*height,1,fp_bmp);
        fclose(fp_rgb24);
        fclose(fp_bmp);
        free(rgb24_buffer);
        printf("Finish generate %s!\n",bmppath);

        return 0;
}

void MainWindow::on_pushButton_8_clicked()
{
    simplest_rgb24_to_bmp("D:\\liuqs_myself\\Qt-pro\\untitled6\\lena_256x256_rgb24.rgb",256,256,"output_lena.bmp");
    ui->pushButton_8->setText("rgb24_to_bmp");
}

unsigned char clip_value(unsigned char x,unsigned char min_val,unsigned char  max_val){
        if(x>max_val){
                return max_val;
        }else if(x<min_val){
                return min_val;
        }else{
                return x;
        }
}

//RGB to YUV420
bool RGB24_TO_YUV420(unsigned char *RgbBuf,int w,int h,unsigned char *yuvBuf)
{
        unsigned char*ptrY, *ptrU, *ptrV, *ptrRGB;
        memset(yuvBuf,0,w*h*3/2);
        ptrY = yuvBuf;
        ptrU = yuvBuf + w*h;
        ptrV = ptrU + (w*h*1/4);
        unsigned char y, u, v, r, g, b;
        for (int j = 0; j<h;j++){
                ptrRGB = RgbBuf + w*j*3 ;
                for (int i = 0;i<w;i++){

                        r = *(ptrRGB++);
                        g = *(ptrRGB++);
                        b = *(ptrRGB++);
                        y = (unsigned char)( ( 66 * r + 129 * g +  25 * b + 128) >> 8) + 16  ;
                        u = (unsigned char)( ( -38 * r -  74 * g + 112 * b + 128) >> 8) + 128 ;
                        v = (unsigned char)( ( 112 * r -  94 * g -  18 * b + 128) >> 8) + 128 ;
                        *(ptrY++) = clip_value(y,0,255);
                        if (j%2==0&&i%2 ==0){
                                *(ptrU++) =clip_value(u,0,255);
                        }
                        else{
                                if (i%2==0){
                                *(ptrV++) =clip_value(v,0,255);
                                }
                        }
                }
        }
        return true;
}

/**
 * Convert RGB24 file to YUV420P file
 * @param url_in  Location of Input RGB file.
 * @param w       Width of Input RGB file.
 * @param h       Height of Input RGB file.
 * @param num     Number of frames to process.
 * @param url_out Location of Output YUV file.
 */
int simplest_rgb24_to_yuv420(char *url_in, int w, int h,int num,char *url_out){
        FILE *fp=fopen(url_in,"rb+");
        FILE *fp1=fopen(url_out,"wb+");

        unsigned char *pic_rgb24=(unsigned char *)malloc(w*h*3);
        unsigned char *pic_yuv420=(unsigned char *)malloc(w*h*3/2);

        for(int i=0;i<num;i++){
                fread(pic_rgb24,1,w*h*3,fp);
                RGB24_TO_YUV420(pic_rgb24,w,h,pic_yuv420);
                fwrite(pic_yuv420,1,w*h*3/2,fp1);
        }

        free(pic_rgb24);
        free(pic_yuv420);
        fclose(fp);
        fclose(fp1);

        return 0;
}

void MainWindow::on_pushButton_9_clicked()
{
    simplest_rgb24_to_yuv420("D:\\liuqs_myself\\Qt-pro\\untitled6\\lena_256x256_rgb24.rgb",256,256,1,"output_lena.yuv");
    ui->pushButton_9->setText("rgb24_to_yuv420");
}

/**
 * Generate RGB24 colorbar.
 * @param width    Width of Output RGB file.
 * @param height   Height of Output RGB file.
 * @param url_out  Location of Output RGB file.
 */
int simplest_rgb24_colorbar(int width, int height,char *url_out){

        unsigned char *data=NULL;
        int barwidth;
        char filename[100]={0};
        FILE *fp=NULL;
        int i=0,j=0;

        data=(unsigned char *)malloc(width*height*3);
        barwidth=width/8;

        if((fp=fopen(url_out,"wb+"))==NULL){
                printf("Error: Cannot create file!");
                return -1;
        }

        for(j=0;j<height;j++){
                for(i=0;i<width;i++){
                        int barnum=i/barwidth;
                        switch(barnum){
                        case 0:{
                                data[(j*width+i)*3+0]=255;
                                data[(j*width+i)*3+1]=255;
                                data[(j*width+i)*3+2]=255;
                                break;
                                   }
                        case 1:{
                                data[(j*width+i)*3+0]=255;
                                data[(j*width+i)*3+1]=255;
                                data[(j*width+i)*3+2]=0;
                                break;
                                   }
                        case 2:{
                                data[(j*width+i)*3+0]=0;
                                data[(j*width+i)*3+1]=255;
                                data[(j*width+i)*3+2]=255;
                                break;
                                   }
                        case 3:{
                                data[(j*width+i)*3+0]=0;
                                data[(j*width+i)*3+1]=255;
                                data[(j*width+i)*3+2]=0;
                                break;
                                   }
                        case 4:{
                                data[(j*width+i)*3+0]=255;
                                data[(j*width+i)*3+1]=0;
                                data[(j*width+i)*3+2]=255;
                                break;
                                   }
                        case 5:{
                                data[(j*width+i)*3+0]=255;
                                data[(j*width+i)*3+1]=0;
                                data[(j*width+i)*3+2]=0;
                                break;
                                   }
                        case 6:{
                                data[(j*width+i)*3+0]=0;
                                data[(j*width+i)*3+1]=0;
                                data[(j*width+i)*3+2]=255;

                                break;
                                   }
                        case 7:{
                                data[(j*width+i)*3+0]=0;
                                data[(j*width+i)*3+1]=0;
                                data[(j*width+i)*3+2]=0;
                                break;
                                   }
                        }

                }
        }
        fwrite(data,width*height*3,1,fp);
        fclose(fp);
        free(data);

        return 0;
}

void MainWindow::on_pushButton_10_clicked()
{
    simplest_rgb24_colorbar(640, 360,"D:\\liuqs_myself\\Qt-pro\\untitled6\\colorbar_640x360.rgb");
    ui->pushButton_10->setText("rgb24_colorbar");
}

/**
 * Split Left and Right channel of 16LE PCM file.
 * @param url  Location of PCM file.
 *
 */
int simplest_pcm16le_split(char *url){
        FILE *fp=fopen(url,"rb+");
        FILE *fp1=fopen("output_l.pcm","wb+");
        FILE *fp2=fopen("output_r.pcm","wb+");

        unsigned char *sample=(unsigned char *)malloc(4);

        while(!feof(fp)){
                fread(sample,1,4,fp);
                //L
                fwrite(sample,1,2,fp1);
                //R
                fwrite(sample+2,1,2,fp2);
        }

        free(sample);
        fclose(fp);
        fclose(fp1);
        fclose(fp2);
        return 0;
}

void MainWindow::on_pushButton_11_clicked()
{
    simplest_pcm16le_split("D:\\liuqs_myself\\Qt-pro\\untitled6\\NocturneNo2inEflat_44.1k_s16le.pcm");
    ui->pushButton_11->setText("pcm16le_split ok");
}
/**
 * Halve volume of Left channel of 16LE PCM file
 * @param url  Location of PCM file.
 */
int simplest_pcm16le_halfvolumeleft(char *url){
        FILE *fp=fopen(url,"rb+");
        FILE *fp1=fopen("output_halfleft.pcm","wb+");

        int cnt=0;

        unsigned char *sample=(unsigned char *)malloc(4);

        while(!feof(fp)){
                short *samplenum=NULL;
                fread(sample,1,4,fp);

                samplenum=(short *)sample;
                *samplenum=*samplenum/2;
                //L
                fwrite(sample,1,2,fp1);
                //R
                fwrite(sample+2,1,2,fp1);

                cnt++;
        }
        printf("Sample Cnt:%d\n",cnt);

        free(sample);
        fclose(fp);
        fclose(fp1);
        return 0;
}
void MainWindow::on_pushButton_12_clicked()
{
simplest_pcm16le_halfvolumeleft("D:\\liuqs_myself\\Qt-pro\\untitled6\\NocturneNo2inEflat_44.1k_s16le.pcm");ui->pushButton_12->setText("pcm16le_split");
}
/**
 * Re-sample to double the speed of 16LE PCM file
 * @param url  Location of PCM file.
 */
int simplest_pcm16le_doublespeed(char *url){
        FILE *fp=fopen(url,"rb+");
        FILE *fp1=fopen("output_doublespeed.pcm","wb+");

        int cnt=0;

        unsigned char *sample=(unsigned char *)malloc(4);

        while(!feof(fp)){

                fread(sample,1,4,fp);

                if(cnt%2!=0){
                        //L
                        fwrite(sample,1,2,fp1);
                        //R
                        fwrite(sample+2,1,2,fp1);
                }
                cnt++;
        }
        printf("Sample Cnt:%d\n",cnt);

        free(sample);
        fclose(fp);
        fclose(fp1);
        return 0;
}
void MainWindow::on_pushButton_13_clicked()
{
    simplest_pcm16le_doublespeed("D:\\liuqs_myself\\Qt-pro\\untitled6\\NocturneNo2inEflat_44.1k_s16le.pcm");
}

/**
 * Convert PCM-16 data to PCM-8 data.
 * @param url  Location of PCM file.
 */
int simplest_pcm16le_to_pcm8(char *url){
        FILE *fp=fopen(url,"rb+");
        FILE *fp1=fopen("output_8.pcm","wb+");

        int cnt=0;

        unsigned char *sample=(unsigned char *)malloc(4);

        while(!feof(fp)){

                short *samplenum16=NULL;
                char samplenum8=0;
                unsigned char samplenum8_u=0;
                fread(sample,1,4,fp);
                //(-32768-32767)
                samplenum16=(short *)sample;
                samplenum8=(*samplenum16)>>8;
                //(0-255)
                samplenum8_u=samplenum8+128;
                //L
                fwrite(&samplenum8_u,1,1,fp1);

                samplenum16=(short *)(sample+2);
                samplenum8=(*samplenum16)>>8;
                samplenum8_u=samplenum8+128;
                //R
                fwrite(&samplenum8_u,1,1,fp1);
                cnt++;
        }
        printf("Sample Cnt:%d\n",cnt);

        free(sample);
        fclose(fp);
        fclose(fp1);
        return 0;
}

void MainWindow::on_pushButton_14_clicked()
{
    simplest_pcm16le_to_pcm8("D:\\liuqs_myself\\Qt-pro\\untitled6\\NocturneNo2inEflat_44.1k_s16le.pcm");
}

/**
 * Cut a 16LE PCM single channel file.
 * @param url        Location of PCM file.
 * @param start_num  start point
 * @param dur_num    how much point to cut
 */
int simplest_pcm16le_cut_singlechannel(char *url,int start_num,int dur_num){
        FILE *fp=fopen(url,"rb+");
        FILE *fp1=fopen("output_cut.pcm","wb+");
        FILE *fp_stat=fopen("output_cut.txt","wb+");

        unsigned char *sample=(unsigned char *)malloc(2);

        int cnt=0;
        while(!feof(fp)){
                fread(sample,1,2,fp);
                if(cnt>start_num&&cnt<=(start_num+dur_num)){
                        fwrite(sample,1,2,fp1);

                        short samplenum=sample[1];
                        samplenum=samplenum*256;
                        samplenum=samplenum+sample[0];

                        fprintf(fp_stat,"%6d,",samplenum);
                        if(cnt%10==0)
                                fprintf(fp_stat,"\n",samplenum);
                }
                cnt++;
        }

        free(sample);
        fclose(fp);
        fclose(fp1);
        fclose(fp_stat);
        return 0;
}

void MainWindow::on_pushButton_15_clicked()
{
    simplest_pcm16le_cut_singlechannel("D:\\liuqs_myself\\Qt-pro\\untitled6\\drum.pcm",2360,120);
}

/**
 * Convert PCM16LE raw data to WAVE format
 * @param pcmpath      Input PCM file.
 * @param channels     Channel number of PCM file.
 * @param sample_rate  Sample rate of PCM file.
 * @param wavepath     Output WAVE file.
 */
int simplest_pcm16le_to_wave(const char *pcmpath,int channels,int sample_rate,const char *wavepath)
{

        typedef struct WAVE_HEADER{
                char         fccID[4];
                unsigned   long    dwSize;
                char         fccType[4];
        }WAVE_HEADER;

        typedef struct WAVE_FMT{
                char         fccID[4];
                unsigned   long       dwSize;
                unsigned   short     wFormatTag;
                unsigned   short     wChannels;
                unsigned   long       dwSamplesPerSec;
                unsigned   long       dwAvgBytesPerSec;
                unsigned   short     wBlockAlign;
                unsigned   short     uiBitsPerSample;
        }WAVE_FMT;

        typedef struct WAVE_DATA{
                char       fccID[4];
                unsigned long dwSize;
        }WAVE_DATA;


        if(channels==0||sample_rate==0){
    channels = 2;
    sample_rate = 44100;
        }
        int bits = 16;

    WAVE_HEADER   pcmHEADER;
    WAVE_FMT   pcmFMT;
    WAVE_DATA   pcmDATA;

    unsigned   short   m_pcmData;
    FILE   *fp,*fpout;

        fp=fopen(pcmpath, "rb");
    if(fp == NULL) {
        printf("open pcm file error\n");
        return -1;
    }
        fpout=fopen(wavepath,   "wb+");
    if(fpout == NULL) {
        printf("create wav file error\n");
        return -1;
    }
        //WAVE_HEADER
    memcpy(pcmHEADER.fccID,"RIFF",strlen("RIFF"));
    memcpy(pcmHEADER.fccType,"WAVE",strlen("WAVE"));
    fseek(fpout,sizeof(WAVE_HEADER),1);
        //WAVE_FMT
    pcmFMT.dwSamplesPerSec=sample_rate;
    pcmFMT.dwAvgBytesPerSec=pcmFMT.dwSamplesPerSec*sizeof(m_pcmData);
    pcmFMT.uiBitsPerSample=bits;
    memcpy(pcmFMT.fccID,"fmt ",strlen("fmt "));
    pcmFMT.dwSize=16;
    pcmFMT.wBlockAlign=2;
    pcmFMT.wChannels=channels;
    pcmFMT.wFormatTag=1;

    fwrite(&pcmFMT,sizeof(WAVE_FMT),1,fpout);

    //WAVE_DATA;
    memcpy(pcmDATA.fccID,"data",strlen("data"));
    pcmDATA.dwSize=0;
    fseek(fpout,sizeof(WAVE_DATA),SEEK_CUR);

    fread(&m_pcmData,sizeof(unsigned short),1,fp);
    while(!feof(fp)){
        pcmDATA.dwSize+=2;
        fwrite(&m_pcmData,sizeof(unsigned short),1,fpout);
        fread(&m_pcmData,sizeof(unsigned short),1,fp);
    }

    pcmHEADER.dwSize=44+pcmDATA.dwSize;

    rewind(fpout);
    fwrite(&pcmHEADER,sizeof(WAVE_HEADER),1,fpout);
    fseek(fpout,sizeof(WAVE_FMT),SEEK_CUR);
    fwrite(&pcmDATA,sizeof(WAVE_DATA),1,fpout);

        fclose(fp);
    fclose(fpout);

    return 0;
}

void MainWindow::on_pushButton_16_clicked()
{
    simplest_pcm16le_to_wave("D:\\liuqs_myself\\Qt-pro\\untitled6\\NocturneNo2inEflat_44.1k_s16le.pcm",2,44100,"output_nocturne.wav");
    ui->pushButton_16->setText("pcm16le_to_wave");
}

/**
 * 最简单的视音频数据处理示例
 * Simplest MediaData Test
 *
 * 雷霄骅 Lei Xiaohua
 * leixiaohua1020@126.com
 * 中国传媒大学/数字电视技术
 * Communication University of China / Digital TV Technology
 * http://blog.csdn.net/leixiaohua1020
 *
 * 本项目包含如下几种视音频测试示例：
 *  (1)像素数据处理程序。包含RGB和YUV像素格式处理的函数。
 *  (2)音频采样数据处理程序。包含PCM音频采样格式处理的函数。
 *  (3)H.264码流分析程序。可以分离并解析NALU。
 *  (4)AAC码流分析程序。可以分离并解析ADTS帧。
 *  (5)FLV封装格式分析程序。可以将FLV中的MP3音频码流分离出来。
 *  (6)UDP-RTP协议分析程序。可以将分析UDP/RTP/MPEG-TS数据包。
 *
 * This project contains following samples to handling multimedia data:
 *  (1) Video pixel data handling program. It contains several examples to handle RGB and YUV data.
 *  (2) Audio sample data handling program. It contains several examples to handle PCM data.
 *  (3) H.264 stream analysis program. It can parse H.264 bitstream and analysis NALU of stream.
 *  (4) AAC stream analysis program. It can parse AAC bitstream and analysis ADTS frame of stream.
 *  (5) FLV format analysis program. It can analysis FLV file and extract MP3 audio stream.
 *  (6) UDP-RTP protocol analysis program. It can analysis UDP/RTP/MPEG-TS Packet.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
        NALU_TYPE_SLICE    = 1,
        NALU_TYPE_DPA      = 2,
        NALU_TYPE_DPB      = 3,
        NALU_TYPE_DPC      = 4,
        NALU_TYPE_IDR      = 5,
        NALU_TYPE_SEI      = 6,
        NALU_TYPE_SPS      = 7,
        NALU_TYPE_PPS      = 8,
        NALU_TYPE_AUD      = 9,
        NALU_TYPE_EOSEQ    = 10,
        NALU_TYPE_EOSTREAM = 11,
        NALU_TYPE_FILL     = 12,
} NaluType;

typedef enum {
        NALU_PRIORITY_DISPOSABLE = 0,
        NALU_PRIRITY_LOW         = 1,
        NALU_PRIORITY_HIGH       = 2,
        NALU_PRIORITY_HIGHEST    = 3
} NaluPriority;


typedef struct
{
        int startcodeprefix_len;      //! 4 for parameter sets and first slice in picture, 3 for everything else (suggested)
        unsigned len;                 //! Length of the NAL unit (Excluding the start code, which does not belong to the NALU)
        unsigned max_size;            //! Nal Unit Buffer size
        int forbidden_bit;            //! should be always FALSE
        int nal_reference_idc;        //! NALU_PRIORITY_xxxx
        int nal_unit_type;            //! NALU_TYPE_xxxx
        char *buf;                    //! contains the first byte followed by the EBSP
} NALU_t;

FILE *h264bitstream = NULL;                //!< the bit stream file

int info2=0, info3=0;

static int FindStartCode2 (unsigned char *Buf){
        if(Buf[0]!=0 || Buf[1]!=0 || Buf[2] !=1) return 0; //0x000001?
        else return 1;
}

static int FindStartCode3 (unsigned char *Buf){
        if(Buf[0]!=0 || Buf[1]!=0 || Buf[2] !=0 || Buf[3] !=1) return 0;//0x00000001?
        else return 1;
}


int GetAnnexbNALU (NALU_t *nalu){
        int pos = 0;
        int StartCodeFound, rewind;
        unsigned char *Buf;

        if ((Buf = (unsigned char*)calloc (nalu->max_size , sizeof(char))) == NULL)
                printf ("GetAnnexbNALU: Could not allocate Buf memory\n");

        nalu->startcodeprefix_len=3;
        qDebug() << "000 nalu->len "<< nalu->len<< "pos" << pos << "nalu->startcodeprefix_len" << nalu->startcodeprefix_len<< endl;

        if (3 != fread (Buf, 1, 3, h264bitstream)){
                free(Buf);
                return 0;
        }
        info2 = FindStartCode2 (Buf);
        if(info2 != 1)
        {
                qDebug() << "000.1 nalu->len "<< nalu->len<< "pos" << pos << "nalu->startcodeprefix_len" << nalu->startcodeprefix_len<< endl;
                if(1 != fread(Buf+3, 1, 1, h264bitstream)){
                        free(Buf);
                        return 0;
                }
                info3 = FindStartCode3 (Buf);
                if (info3 != 1){
                        free(Buf);
                        return -1;
                }
                else {
                        pos = 4;
                        nalu->startcodeprefix_len = 4;
                }
                qDebug() << "00.2 nalu->len "<< nalu->len<< "pos" << pos << "nalu->startcodeprefix_len" << nalu->startcodeprefix_len<< endl;
        }
        else{
                nalu->startcodeprefix_len = 3;
                pos = 3;
                qDebug() << "00.3 nalu->len "<< nalu->len<< "pos" << pos << "nalu->startcodeprefix_len" << nalu->startcodeprefix_len<< endl;
        }
         qDebug() << "001 nalu->len "<< nalu->len<< "pos" << pos << "nalu->startcodeprefix_len" << nalu->startcodeprefix_len<< endl;

        StartCodeFound = 0;
        info2 = 0;
        info3 = 0;

        while (!StartCodeFound){
                if (feof (h264bitstream)){
                        nalu->len = (pos-1)-nalu->startcodeprefix_len;
                        //qDebug() << "002 nalu->len "<< nalu->len<< "pos" << pos << "nalu->startcodeprefix_len" << nalu->startcodeprefix_len<< endl;
                        memcpy (nalu->buf, &Buf[nalu->startcodeprefix_len], nalu->len);
                        nalu->forbidden_bit = nalu->buf[0] & 0x80; //1 bit
                        nalu->nal_reference_idc = nalu->buf[0] & 0x60; // 2 bit
                        nalu->nal_unit_type = (nalu->buf[0]) & 0x1f;// 5 bit
                        free(Buf);
                        return pos-1;
                }
                Buf[pos++] = fgetc (h264bitstream);
                info3 = FindStartCode3(&Buf[pos-4]);
                if(info3 != 1)
                        info2 = FindStartCode2(&Buf[pos-3]);
                StartCodeFound = (info2 == 1 || info3 == 1);
        }

        // Here, we have found another start code (and read length of startcode bytes more than we should
        // have.  Hence, go back in the file
        rewind = (info3 == 1)? -4 : -3;

        if (0 != fseek (h264bitstream, rewind, SEEK_CUR)){
                free(Buf);
                printf("GetAnnexbNALU: Cannot fseek in the bit stream file");
        }

        // Here the Start code, the complete NALU, and the next start code is in the Buf.
        // The size of Buf is pos, pos+rewind are the number of bytes excluding the next
        // start code, and (pos+rewind)-startcodeprefix_len is the size of the NALU excluding the start code

        nalu->len = (pos+rewind)-nalu->startcodeprefix_len;
        memcpy (nalu->buf, &Buf[nalu->startcodeprefix_len], nalu->len);//
        nalu->forbidden_bit = nalu->buf[0] & 0x80; //1 bit
        nalu->nal_reference_idc = nalu->buf[0] & 0x60; // 2 bit
        nalu->nal_unit_type = (nalu->buf[0]) & 0x1f;// 5 bit
        free(Buf);

        //qDebug() << "003 rewind============================"<< rewind << "pos" << pos << endl;

        return (pos+rewind);
}

/**
 * Analysis H.264 Bitstream
 * @param url    Location of input H.264 bitstream file.
 */
int simplest_h264_parser(char *url){

        NALU_t *n;
        int buffersize=100000;

        FILE *myout=fopen("output_log.txt","wb+");
        //FILE *myout=stdout;

        h264bitstream=fopen(url, "rb+");
        if (h264bitstream==NULL){
                printf("Open file error\n");
                return 0;
        }

        n = (NALU_t*)calloc (1, sizeof (NALU_t));
        if (n == NULL){
                printf("Alloc NALU Error\n");
                return 0;
        }

        n->max_size=buffersize;
        n->buf = (char*)calloc (buffersize, sizeof (char));
        if (n->buf == NULL){
                free (n);
                printf ( "AllocNALU: n->buf" );
                return 0;
        }

        int data_offset=0;
        int nal_num=0;
        printf("-----+-------- NALU Table ------+---------+\n");
        printf(" NUM |    POS  |    IDC |  TYPE |   LEN   |\n");
        printf("-----+---------+--------+-------+---------+\n");

        while(!feof(h264bitstream))
        {
                int data_lenth;
                data_lenth=GetAnnexbNALU(n);
                qDebug() << "data_lenth" << data_lenth << "n->nal_unit_type" << n->nal_unit_type << endl;
                qDebug() << "n->nal_reference_idc>>5" << (n->nal_reference_idc >> 5) << endl;
                char type_str[20]={0};
                switch(n->nal_unit_type){
                        case NALU_TYPE_SLICE:sprintf(type_str,"SLICE");break;
                        case NALU_TYPE_DPA:sprintf(type_str,"DPA");break;
                        case NALU_TYPE_DPB:sprintf(type_str,"DPB");break;
                        case NALU_TYPE_DPC:sprintf(type_str,"DPC");break;
                        case NALU_TYPE_IDR:sprintf(type_str,"IDR");break;
                        case NALU_TYPE_SEI:sprintf(type_str,"SEI");break;
                        case NALU_TYPE_SPS:sprintf(type_str,"SPS");break;
                        case NALU_TYPE_PPS:sprintf(type_str,"PPS");break;
                        case NALU_TYPE_AUD:sprintf(type_str,"AUD");break;
                        case NALU_TYPE_EOSEQ:sprintf(type_str,"EOSEQ");break;
                        case NALU_TYPE_EOSTREAM:sprintf(type_str,"EOSTREAM");break;
                        case NALU_TYPE_FILL:sprintf(type_str,"FILL");break;
                }
                char idc_str[20]={0};
                switch(n->nal_reference_idc>>5){
                        case NALU_PRIORITY_DISPOSABLE:sprintf(idc_str,"DISPOS");break;
                        case NALU_PRIRITY_LOW:sprintf(idc_str,"LOW");break;
                        case NALU_PRIORITY_HIGH:sprintf(idc_str,"HIGH");break;
                        case NALU_PRIORITY_HIGHEST:sprintf(idc_str,"HIGHEST");break;
                }

                fprintf(myout,"%5d| %8d| %7s| %6s| %8d|\n",nal_num,data_offset,idc_str,type_str,n->len);

                data_offset=data_offset+data_lenth;

                nal_num++;
        }

        //Free
        if (n){
                if (n->buf){
                        free(n->buf);
                        n->buf=NULL;
                }
                free (n);
        }
        return 0;
}

void MainWindow::on_pushButton_17_clicked()
{
    simplest_h264_parser("D:\\liuqs_myself\\Qt-pro\\untitled6\\sintel.h264");
    ui->pushButton_17->setText("h264_parser ok");
}

/**
 * 最简单的视音频数据处理示例
 * Simplest MediaData Test
 *
 * 雷霄骅 Lei Xiaohua
 * leixiaohua1020@126.com
 * 中国传媒大学/数字电视技术
 * Communication University of China / Digital TV Technology
 * http://blog.csdn.net/leixiaohua1020
 *
 * 本项目包含如下几种视音频测试示例：
 *  (1)像素数据处理程序。包含RGB和YUV像素格式处理的函数。
 *  (2)音频采样数据处理程序。包含PCM音频采样格式处理的函数。
 *  (3)H.264码流分析程序。可以分离并解析NALU。
 *  (4)AAC码流分析程序。可以分离并解析ADTS帧。
 *  (5)FLV封装格式分析程序。可以将FLV中的MP3音频码流分离出来。
 *  (6)UDP-RTP协议分析程序。可以将分析UDP/RTP/MPEG-TS数据包。
 *
 * This project contains following samples to handling multimedia data:
 *  (1) Video pixel data handling program. It contains several examples to handle RGB and YUV data.
 *  (2) Audio sample data handling program. It contains several examples to handle PCM data.
 *  (3) H.264 stream analysis program. It can parse H.264 bitstream and analysis NALU of stream.
 *  (4) AAC stream analysis program. It can parse AAC bitstream and analysis ADTS frame of stream.
 *  (5) FLV format analysis program. It can analysis FLV file and extract MP3 audio stream.
 *  (6) UDP-RTP protocol analysis program. It can analysis UDP/RTP/MPEG-TS Packet.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int getADTSframe(unsigned char* buffer, int buf_size, unsigned char* data ,int* data_size){
        int size = 0;

        if(!buffer || !data || !data_size ){
                return -1;
        }

        while(1){
                if(buf_size  < 7 ){
                        return -1;
                }
                //Sync words
                if((buffer[0] == 0xff) && ((buffer[1] & 0xf0) == 0xf0) ){
                        size |= ((buffer[3] & 0x03) <<11);     //high 2 bit
                        size |= buffer[4]<<3;                //middle 8 bit
                        size |= ((buffer[5] & 0xe0)>>5);        //low 3bit
                        break;
                }
                --buf_size;
                ++buffer;
        }

        if(buf_size < size){
                return 1;
        }

        memcpy(data, buffer, size);
        *data_size = size;

        return 0;
}

int simplest_aac_parser(char *url)
{
        int data_size = 0;
        int size = 0;
        int cnt=0;
        int offset=0;

        FILE *myout=fopen("output_log.txt","wb+");
        //FILE *myout=stdout;

        unsigned char *aacframe=(unsigned char *)malloc(1024*5);
        unsigned char *aacbuffer=(unsigned char *)malloc(1024*1024);

        FILE *ifile = fopen(url, "rb");
        if(!ifile){
                printf("Open file error");
                return -1;
        }

        printf("-----+- ADTS Frame Table -+------+\n");
        printf(" NUM | Profile | Frequency| Size |\n");
        printf("-----+---------+----------+------+\n");

        while(!feof(ifile)){
                data_size = fread(aacbuffer+offset, 1, 1024*1024-offset, ifile);
                unsigned char* input_data = aacbuffer;

                while(1)
                {
                        int ret=getADTSframe(input_data, data_size, aacframe, &size);
                        if(ret==-1){
                                break;
                        }else if(ret==1){
                                memcpy(aacbuffer,input_data,data_size);
                                offset=data_size;
                                break;
                        }

                        char profile_str[10]={0};
                        char frequence_str[10]={0};

                        unsigned char profile=aacframe[2]&0xC0;
                        profile=profile>>6;
                        switch(profile){
                        case 0: sprintf(profile_str,"Main");break;
                        case 1: sprintf(profile_str,"LC");break;
                        case 2: sprintf(profile_str,"SSR");break;
                        default:sprintf(profile_str,"unknown");break;
                        }

                        unsigned char sampling_frequency_index=aacframe[2]&0x3C;
                        sampling_frequency_index=sampling_frequency_index>>2;
                        switch(sampling_frequency_index){
                        case 0: sprintf(frequence_str,"96000Hz");break;
                        case 1: sprintf(frequence_str,"88200Hz");break;
                        case 2: sprintf(frequence_str,"64000Hz");break;
                        case 3: sprintf(frequence_str,"48000Hz");break;
                        case 4: sprintf(frequence_str,"44100Hz");break;
                        case 5: sprintf(frequence_str,"32000Hz");break;
                        case 6: sprintf(frequence_str,"24000Hz");break;
                        case 7: sprintf(frequence_str,"22050Hz");break;
                        case 8: sprintf(frequence_str,"16000Hz");break;
                        case 9: sprintf(frequence_str,"12000Hz");break;
                        case 10: sprintf(frequence_str,"11025Hz");break;
                        case 11: sprintf(frequence_str,"8000Hz");break;
                        default:sprintf(frequence_str,"unknown");break;
                        }


                        fprintf(myout,"%5d| %8s|  %8s| %5d|\n",cnt,profile_str ,frequence_str,size);
                        data_size -= size;
                        input_data += size;
                        cnt++;
                }

        }
        fclose(ifile);
        free(aacbuffer);
        free(aacframe);

        return 0;
}

void MainWindow::on_pushButton_18_clicked()
{
    simplest_aac_parser("D:\\liuqs_myself\\Qt-pro\\untitled6\\nocturne.aac");
    ui->pushButton_18->setText("aac_parser ok");
}

/**
 * 最简单的视音频数据处理示例
 * Simplest MediaData Test
 *
 * 雷霄骅 Lei Xiaohua
 * leixiaohua1020@126.com
 * 中国传媒大学/数字电视技术
 * Communication University of China / Digital TV Technology
 * http://blog.csdn.net/leixiaohua1020
 *
 * 本项目包含如下几种视音频测试示例：
 *  (1)像素数据处理程序。包含RGB和YUV像素格式处理的函数。
 *  (2)音频采样数据处理程序。包含PCM音频采样格式处理的函数。
 *  (3)H.264码流分析程序。可以分离并解析NALU。
 *  (4)AAC码流分析程序。可以分离并解析ADTS帧。
 *  (5)FLV封装格式分析程序。可以将FLV中的MP3音频码流分离出来。
 *  (6)UDP-RTP协议分析程序。可以将分析UDP/RTP/MPEG-TS数据包。
 *
 * This project contains following samples to handling multimedia data:
 *  (1) Video pixel data handling program. It contains several examples to handle RGB and YUV data.
 *  (2) Audio sample data handling program. It contains several examples to handle PCM data.
 *  (3) H.264 stream analysis program. It can parse H.264 bitstream and analysis NALU of stream.
 *  (4) AAC stream analysis program. It can parse AAC bitstream and analysis ADTS frame of stream.
 *  (5) FLV format analysis program. It can analysis FLV file and extract MP3 audio stream.
 *  (6) UDP-RTP protocol analysis program. It can analysis UDP/RTP/MPEG-TS Packet.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Important!
#pragma pack(1)


#define TAG_TYPE_SCRIPT 18
#define TAG_TYPE_AUDIO  8
#define TAG_TYPE_VIDEO  9

typedef unsigned char byte;
typedef unsigned int uint;

typedef struct {
        byte Signature[3];
        byte Version;
        byte Flags;
        uint DataOffset;
} FLV_HEADER;

typedef struct {
        byte TagType;
        byte DataSize[3];
        byte Timestamp[3];
        uint Reserved;
} TAG_HEADER;


//reverse_bytes - turn a BigEndian byte array into a LittleEndian integer
uint reverse_bytes(byte *p, char c) {
        int r = 0;
        int i;
        for (i=0; i<c; i++)
                r |= ( *(p+i) << (((c-1)*8)-8*i));
        return r;
}

/**
 * Analysis FLV file
 * @param url    Location of input FLV file.
 */

int simplest_flv_parser(char *url){

        //whether output audio/video stream
        int output_a=1;
        int output_v=1;
        //-------------
        FILE *ifh=NULL,*vfh=NULL, *afh = NULL;

        FILE *myout=fopen("output_log.txt","wb+");
        //FILE *myout=stdout;

        FLV_HEADER flv;
        TAG_HEADER tagheader;
        uint previoustagsize, previoustagsize_z=0;
        uint ts=0, ts_new=0;

        ifh = fopen(url, "rb+");
        if ( ifh== NULL) {
                printf("Failed to open files!");
                return -1;
        }

        //FLV file header
        fread((char *)&flv,1,sizeof(FLV_HEADER),ifh);

        fprintf(myout,"============== FLV Header ==============\n");
        fprintf(myout,"Signature:  0x %c %c %c\n",flv.Signature[0],flv.Signature[1],flv.Signature[2]);
        fprintf(myout,"Version:    0x %X\n",flv.Version);
        fprintf(myout,"Flags  :    0x %X\n",flv.Flags);
        fprintf(myout,"HeaderSize: 0x %X\n",reverse_bytes((byte *)&flv.DataOffset, sizeof(flv.DataOffset)));
        fprintf(myout,"========================================\n");

        //move the file pointer to the end of the header
        fseek(ifh, reverse_bytes((byte *)&flv.DataOffset, sizeof(flv.DataOffset)), SEEK_SET);

        //process each tag
        do {

                previoustagsize = _getw(ifh);

                fread((void *)&tagheader,sizeof(TAG_HEADER),1,ifh);

                //int temp_datasize1=reverse_bytes((byte *)&tagheader.DataSize, sizeof(tagheader.DataSize));
                int tagheader_datasize=tagheader.DataSize[0]*65536+tagheader.DataSize[1]*256+tagheader.DataSize[2];
                int tagheader_timestamp=tagheader.Timestamp[0]*65536+tagheader.Timestamp[1]*256+tagheader.Timestamp[2];

                char tagtype_str[10];
                switch(tagheader.TagType){
                case TAG_TYPE_AUDIO:sprintf(tagtype_str,"AUDIO");break;
                case TAG_TYPE_VIDEO:sprintf(tagtype_str,"VIDEO");break;
                case TAG_TYPE_SCRIPT:sprintf(tagtype_str,"SCRIPT");break;
                default:sprintf(tagtype_str,"UNKNOWN");break;
                }
                fprintf(myout,"tagtype_str:[%6s] size:%6d time:%6d |",tagtype_str,tagheader_datasize,tagheader_timestamp);

                //if we are not past the end of file, process the tag
                if (feof(ifh)) {
                        break;
                }

                //process tag by type
                switch (tagheader.TagType) {

                case TAG_TYPE_AUDIO:{
                        char audiotag_str[100]={0};
                        strcat(audiotag_str,"| ");
                        char tagdata_first_byte;
                        tagdata_first_byte=fgetc(ifh);
                        int x=tagdata_first_byte&0xF0;
                        x=x>>4;
                        switch (x)
                        {
                        case 0:strcat(audiotag_str,"Linear PCM, platform endian");break;
                        case 1:strcat(audiotag_str,"ADPCM");break;
                        case 2:strcat(audiotag_str,"MP3");break;
                        case 3:strcat(audiotag_str,"Linear PCM, little endian");break;
                        case 4:strcat(audiotag_str,"Nellymoser 16-kHz mono");break;
                        case 5:strcat(audiotag_str,"Nellymoser 8-kHz mono");break;
                        case 6:strcat(audiotag_str,"Nellymoser");break;
                        case 7:strcat(audiotag_str,"G.711 A-law logarithmic PCM");break;
                        case 8:strcat(audiotag_str,"G.711 mu-law logarithmic PCM");break;
                        case 9:strcat(audiotag_str,"reserved");break;
                        case 10:strcat(audiotag_str,"AAC");break;
                        case 11:strcat(audiotag_str,"Speex");break;
                        case 14:strcat(audiotag_str,"MP3 8-Khz");break;
                        case 15:strcat(audiotag_str,"Device-specific sound");break;
                        default:strcat(audiotag_str,"UNKNOWN");break;
                        }
                        strcat(audiotag_str,"| ");
                        x=tagdata_first_byte&0x0C;
                        x=x>>2;
                        switch (x)
                        {
                        case 0:strcat(audiotag_str,"5.5-kHz");break;
                        case 1:strcat(audiotag_str,"1-kHz");break;
                        case 2:strcat(audiotag_str,"22-kHz");break;
                        case 3:strcat(audiotag_str,"44-kHz");break;
                        default:strcat(audiotag_str,"UNKNOWN");break;
                        }
                        strcat(audiotag_str,"| ");
                        x=tagdata_first_byte&0x02;
                        x=x>>1;
                        switch (x)
                        {
                        case 0:strcat(audiotag_str,"8Bit");break;
                        case 1:strcat(audiotag_str,"16Bit");break;
                        default:strcat(audiotag_str,"UNKNOWN");break;
                        }
                        strcat(audiotag_str,"| ");
                        x=tagdata_first_byte&0x01;
                        switch (x)
                        {
                        case 0:strcat(audiotag_str,"Mono");break;
                        case 1:strcat(audiotag_str,"Stereo");break;
                        default:strcat(audiotag_str,"UNKNOWN");break;
                        }
                        fprintf(myout,"%s",audiotag_str);

                        //if the output file hasn't been opened, open it.
                        if(output_a!=0&&afh == NULL){
                                afh = fopen("output.mp3", "wb");
                        }

                        //TagData - First Byte Data
                        int data_size=reverse_bytes((byte *)&tagheader.DataSize, sizeof(tagheader.DataSize))-1;
                        if(output_a!=0){
                                //TagData+1
                                for (int i=0; i<data_size; i++)
                                        fputc(fgetc(ifh),afh);

                        }else{
                                for (int i=0; i<data_size; i++)
                                        fgetc(ifh);
                        }
                        break;
                }
                case TAG_TYPE_VIDEO:{
                        char videotag_str[100]={0};
                        strcat(videotag_str,"| ");
                        char tagdata_first_byte;
                        tagdata_first_byte=fgetc(ifh);
                        int x=tagdata_first_byte&0xF0;
                        x=x>>4;
                        switch (x)
                        {
                        case 1:strcat(videotag_str,"key frame  ");break;
                        case 2:strcat(videotag_str,"inter frame");break;
                        case 3:strcat(videotag_str,"disposable inter frame");break;
                        case 4:strcat(videotag_str,"generated keyframe");break;
                        case 5:strcat(videotag_str,"video info/command frame");break;
                        default:strcat(videotag_str,"UNKNOWN");break;
                        }
                        strcat(videotag_str,"| ");
                        x=tagdata_first_byte&0x0F;
                        switch (x)
                        {
                        case 1:strcat(videotag_str,"JPEG (currently unused)");break;
                        case 2:strcat(videotag_str,"Sorenson H.263");break;
                        case 3:strcat(videotag_str,"Screen video");break;
                        case 4:strcat(videotag_str,"On2 VP6");break;
                        case 5:strcat(videotag_str,"On2 VP6 with alpha channel");break;
                        case 6:strcat(videotag_str,"Screen video version 2");break;
                        case 7:strcat(videotag_str,"AVC");break;
                        default:strcat(videotag_str,"UNKNOWN");break;
                        }
                        fprintf(myout,"%s",videotag_str);

                        fseek(ifh, -1, SEEK_CUR);
                        //if the output file hasn't been opened, open it.
                        if (vfh == NULL&&output_v!=0) {
                                //write the flv header (reuse the original file's hdr) and first previoustagsize
                                        vfh = fopen("output.flv", "wb");
                                        fwrite((char *)&flv,1, sizeof(flv),vfh);
                                        fwrite((char *)&previoustagsize_z,1,sizeof(previoustagsize_z),vfh);
                        }
#if 0
                        //Change Timestamp
                        //Get Timestamp
                        ts = reverse_bytes((byte *)&tagheader.Timestamp, sizeof(tagheader.Timestamp));
                        ts=ts*2;
                        //Writeback Timestamp
                        ts_new = reverse_bytes((byte *)&ts, sizeof(ts));
                        memcpy(&tagheader.Timestamp, ((char *)&ts_new) + 1, sizeof(tagheader.Timestamp));
#endif


                        //TagData + Previous Tag Size
                        int data_size=reverse_bytes((byte *)&tagheader.DataSize, sizeof(tagheader.DataSize))+4;
                        if(output_v!=0){
                                //TagHeader
                                fwrite((char *)&tagheader,1, sizeof(tagheader),vfh);
                                //TagData
                                for (int i=0; i<data_size; i++)
                                        fputc(fgetc(ifh),vfh);
                        }else{
                                for (int i=0; i<data_size; i++)
                                        fgetc(ifh);
                        }
                        //rewind 4 bytes, because we need to read the previoustagsize again for the loop's sake
                        fseek(ifh, -4, SEEK_CUR);

                        break;
                        }
                default:

                        //skip the data of this tag
                        fseek(ifh, reverse_bytes((byte *)&tagheader.DataSize, sizeof(tagheader.DataSize)), SEEK_CUR);
                }

                fprintf(myout,"\n");

        } while (!feof(ifh));


        _fcloseall();

        return 0;
}

void MainWindow::on_pushButton_19_clicked()
{
    simplest_flv_parser("D:\\liuqs_myself\\Qt-pro\\untitled6\\cuc_ieschool.flv");
    ui->pushButton_19->setText("flv_parser ok");
}

/**
 * 最简单的视音频数据处理示例
 * Simplest MediaData Test
 *
 * 雷霄骅 Lei Xiaohua
 * leixiaohua1020@126.com
 * 中国传媒大学/数字电视技术
 * Communication University of China / Digital TV Technology
 * http://blog.csdn.net/leixiaohua1020
 *
 * 本项目包含如下几种视音频测试示例：
 *  (1)像素数据处理程序。包含RGB和YUV像素格式处理的函数。
 *  (2)音频采样数据处理程序。包含PCM音频采样格式处理的函数。
 *  (3)H.264码流分析程序。可以分离并解析NALU。
 *  (4)AAC码流分析程序。可以分离并解析ADTS帧。
 *  (5)FLV封装格式分析程序。可以将FLV中的MP3音频码流分离出来。
 *  (6)UDP-RTP协议分析程序。可以将分析UDP/RTP/MPEG-TS数据包。
 *
 * This project contains following samples to handling multimedia data:
 *  (1) Video pixel data handling program. It contains several examples to handle RGB and YUV data.
 *  (2) Audio sample data handling program. It contains several examples to handle PCM data.
 *  (3) H.264 stream analysis program. It can parse H.264 bitstream and analysis NALU of stream.
 *  (4) AAC stream analysis program. It can parse AAC bitstream and analysis ADTS frame of stream.
 *  (5) FLV format analysis program. It can analysis FLV file and extract MP3 audio stream.
 *  (6) UDP-RTP protocol analysis program. It can analysis UDP/RTP/MPEG-TS Packet.
 *
 */
#include <stdio.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib") //ws2_32

#pragma pack(1)

/*
 * [memo] FFmpeg stream Command:
 * ffmpeg -re -i sintel.ts -f mpegts udp://127.0.0.1:8880
 * ffmpeg -re -i sintel.ts -f rtp_mpegts udp://127.0.0.1:8880
 */

typedef struct RTP_FIXED_HEADER{
        /* byte 0 */
        unsigned char csrc_len:4;       /* expect 0 */
        unsigned char extension:1;      /* expect 1 */
        unsigned char padding:1;        /* expect 0 */
        unsigned char version:2;        /* expect 2 */
        /* byte 1 */
        unsigned char payload:7;
        unsigned char marker:1;        /* expect 1 */
        /* bytes 2, 3 */
        unsigned short seq_no;
        /* bytes 4-7 */
        unsigned  long timestamp;
        /* bytes 8-11 */
        unsigned long ssrc;            /* stream number is used here. */
} RTP_FIXED_HEADER;

typedef struct MPEGTS_FIXED_HEADER {
        unsigned sync_byte: 8;
        unsigned transport_error_indicator: 1;
        unsigned payload_unit_start_indicator: 1;
        unsigned transport_priority: 1;
        unsigned PID: 13;
        unsigned scrambling_control: 2;
        unsigned adaptation_field_exist: 2;
        unsigned continuity_counter: 4;
} MPEGTS_FIXED_HEADER;



int simplest_udp_parser(int port)
{
        WSADATA wsaData;
        WORD sockVersion = MAKEWORD(2,2);
        int cnt=0;

        FILE *myout=fopen("output_log.txt","wb+");
        //FILE *myout=stdout;

        FILE *fp1=fopen("output_dump.ts","wb+");

        if(WSAStartup(sockVersion, &wsaData) != 0){
                return 0;
        }

        SOCKET serSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if(serSocket == INVALID_SOCKET){
                printf("socket error !");
                return 0;
        }

        sockaddr_in serAddr;
        serAddr.sin_family = AF_INET;
        serAddr.sin_port = htons(port);
       //serAddr.sin_addr.S_un.S_addr = INADDR_ANY; inet_addr
        serAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

        if(bind(serSocket, (sockaddr *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR){
                printf("bind error !");
                closesocket(serSocket);
                return 0;
        }

        sockaddr_in remoteAddr;
        int nAddrLen = sizeof(remoteAddr);

        //How to parse?
        int parse_rtp=1;
        int parse_mpegts=1;


        //printf("Ip : %s.\n",inet_ntoa(peer_addr.sin_addr));

        char recvData[10000];
        while (1)
        {
                qDebug() << "GDU001.1 while..." << endl;
                qDebug() << "GDU001 port:%d" << port  << endl;

                int pktsize = recvfrom(serSocket, recvData, 10000, 0, (sockaddr *)&remoteAddr, &nAddrLen);
                if (pktsize > 0){
                        //printf("Addr:%s\r\n",inet_ntoa(remoteAddr.sin_addr));
                        //printf("packet size:%d\r\n",pktsize);
                        //Parse RTP
                        qDebug() << "GDU002 pktsize:%d" << pktsize << endl;

                        if(parse_rtp!=0)
                        {
                                char payload_str[10]={0};
                                RTP_FIXED_HEADER rtp_header;
                                int rtp_header_size=sizeof(RTP_FIXED_HEADER);
                                //RTP Header
                                memcpy((void *)&rtp_header,recvData,rtp_header_size);

                                //RFC3551
                                char payload=rtp_header.payload;
                                switch(payload){
                                case 0:
                                case 1:
                                case 2:
                                case 3:
                                case 4:
                                case 5:
                                case 6:
                                case 7:
                                case 8:
                                case 9:
                                case 10:
                                case 11:
                                case 12:
                                case 13:
                                case 14:
                                case 15:
                                case 16:
                                case 17:
                                case 18: sprintf(payload_str,"Audio");break;
                                case 31: sprintf(payload_str,"H.261");break;
                                case 32: sprintf(payload_str,"MPV");break;
                                case 33: sprintf(payload_str,"MP2T");break;
                                case 34: sprintf(payload_str,"H.263");break;
                                case 96: sprintf(payload_str,"H.264");break;
                                default:sprintf(payload_str,"other");break;
                                }

                                unsigned int timestamp=ntohl(rtp_header.timestamp);
                                unsigned int seq_no=ntohs(rtp_header.seq_no);

                                fprintf(myout,"[RTP Pkt] %5d| %5s| %10u| %5d| %5d|\n",cnt,payload_str,timestamp,seq_no,pktsize);

                                //RTP Data
                                char *rtp_data=recvData+rtp_header_size;
                                int rtp_data_size=pktsize-rtp_header_size;
                                fwrite(rtp_data,rtp_data_size,1,fp1);

                                //Parse MPEGTS
                                if(parse_mpegts!=0&&payload==33){
                                        MPEGTS_FIXED_HEADER mpegts_header;
                                        for(int i=0;i<rtp_data_size;i=i+188){
                                                if(rtp_data[i]!=0x47)
                                                        break;
                                                //MPEGTS Header
                                                //memcpy((void *)&mpegts_header,rtp_data+i,sizeof(MPEGTS_FIXED_HEADER));
                                                fprintf(myout,"   [MPEGTS Pkt]\n");
                                        }
                                }

                        }
                        else
                        {
                                fprintf(myout,"[UDP Pkt] %5d| %5d|\n",cnt,pktsize);
                                fwrite(recvData,pktsize,1,fp1);
                                qDebug() << "GDU003 test" << endl;
                        }

                        cnt++;
                }
        }
        closesocket(serSocket);
        WSACleanup();
        fclose(fp1);
        qDebug() << "GDU004 end" << endl;

        return 0;
}

void MainWindow::on_pushButton_20_clicked()
{
    simplest_udp_parser(8880);
    ui->pushButton_20->setText("udp_parser ok");
}

void MainWindow::on_pushButton_21_clicked()
{
    unsigned short flytime= 300;
    unsigned char data[8] = {0};
    data[0] = flytime >> 8;
    data[1] = flytime & 0xff;
    qDebug() << "data[0]:%d" << data[0] << "data[1]:%d" << data[1] << endl;
    qDebug() << "sizeof(int32_t):%d" <<sizeof(int32_t) << "uint32_t:%d" <<sizeof(uint32_t) << endl;
}

#if 1
QString getHostIpAddress()
{
    QString strIpAddress;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    // 获取第一个本主机的IPv4地址
    int nListSize = ipAddressesList.size();
    for (int i = 0; i < nListSize; ++i)
    {
           if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
               ipAddressesList.at(i).toIPv4Address()) {
               strIpAddress = ipAddressesList.at(i).toString();
               break;
           }
     }
     // 如果没有找到，则以本地IP地址为IP
     if (strIpAddress.isEmpty())
        strIpAddress = QHostAddress(QHostAddress::LocalHost).toString();

     qDebug() << strIpAddress << endl;
     return strIpAddress;
}

QString getHostMacAddress()
{
    QList<QNetworkInterface> nets = QNetworkInterface::allInterfaces();// 获取所有网络接口列表
    int nCnt = nets.count();
    QString strMacAddr = "";
    for(int i = 0; i < nCnt; i ++)
    {
        // 如果此网络接口被激活并且正在运行并且不是回环地址，则就是我们需要找的Mac地址
        if(nets[i].flags().testFlag(QNetworkInterface::IsUp) && nets[i].flags().testFlag(QNetworkInterface::IsRunning) && !nets[i].flags().testFlag(QNetworkInterface::IsLoopBack))
        {
            strMacAddr = nets[i].hardwareAddress();
            break;
        }
    }
    return strMacAddr;
}
#endif


void MainWindow::init_show()
{
    QDateTime local(QDateTime::currentDateTime());
    QString localTime = local.toString("yyyy-MM-dd:hh:mm:ss");
    //qDebug() << localTime;
    ui->label_time->setText("time:"+localTime);

    ui->label_IP->setText("IP:"+getHostIpAddress());

    ui->label_MAC->setText("MAC:"+getHostMacAddress());

    QString localHostName = QHostInfo::localHostName();
    ui->label_hostname->setText("Hostname:"+localHostName);
}
