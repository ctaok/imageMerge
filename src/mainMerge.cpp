#include <stdio.h>
#include <vector>
#include "OpenCV.h"
using namespace std;

int main(int argc, char** argv)
{
	if (argc != 4)
	{
		cout<<"usage:./imageMerge inputlist outputdir thd"<<endl;
		return -1;
	}
	int cnt = 0;
	char file_name[1024] = {0};
	vector<vector<int> > feats_vec;
	vector<char*> imgs_path;
	FILE *fp = fopen(argv[1], "rt");
	if (fp == NULL) {
		printf("Cannot open file %s to read!\n", argv[1]);
	}
	fgets(file_name, 1024, fp);
	int totlenum = atoi(file_name);
	int *flagArr = new int[totlenum];
	for (int k=0; k<totlenum; k++)
		flagArr[k] = 0;
	while (!feof(fp)) {
		memset(file_name, '\0', 1024);
		fgets(file_name, 1024, fp);
		char *tmp = new char[1024];
		if (file_name == NULL) {
			memcpy(tmp, file_name, 1024);
			continue;
		}
		char *p = file_name;
		while (*p++) {
			if (p[0] == '\r' || p[0] == '\n')
				p[0] = '\0';
		}
		memcpy(tmp, file_name, 1024);
		imgs_path.push_back(tmp);
		IplImage* iplimg = cvLoadImage(file_name, -1);
		if (iplimg == NULL)
			continue;
		flagArr[cnt++] = 1;
		int statis[256] = {0};
		int feat[17] = {0};
		vector<int> feat_one;
		uchar *pimg = (uchar*)(iplimg->imageData);
		for (int i=0; i<iplimg->height; i++) {
			for (int j=0; j<iplimg->widthStep; j++) {
				statis[pimg[i*iplimg->widthStep+j]]++;
			}
		}
		for (int k=0; k<256; k++) {
			int rem = k%16;
			int hol = k-rem;
			feat[hol/16] += cvRound((1-rem/16.)*statis[k]);
			feat[hol/16+1] += cvRound(rem/16.*statis[k]);
		}
		for (int k=0; k<17; k++)
			feat_one.push_back(feat[k]);
		feats_vec.push_back(feat_one);
		cvReleaseImage(&iplimg);
	}
	fclose(fp);
	for (int i=0 ;i<cnt; i++) {
		if (flagArr[i] == 0)
			continue;
		for (int j=i+1; j<cnt; j++) {
			int totle = 0;
			if (flagArr[j] == 0)
				continue;
			for (int k=0; k<17; k++)
				totle += (feats_vec[i][k]-feats_vec[j][k])*(feats_vec[i][k]-feats_vec[j][k]);
			if (totle < atoi(argv[3]))
				flagArr[j] = 0;
		}
	}
	for (int k=0; k<cnt; k++) {
		if (flagArr[k] == 1) {
			IplImage* iplimg = cvLoadImage(imgs_path[k], -1);
			string tmpstr(imgs_path[k]);
			char* tmp1 = imgs_path[k]+tmpstr.rfind('/');
			char tmp2[1024];
			memcpy(tmp2, argv[2], 1024);
			strcat(tmp2, tmp1);
			cvSaveImage(tmp2, iplimg);
			cvReleaseImage(&iplimg);
		}
		delete imgs_path[k];
	}

	delete [] flagArr;
	{
		vector<vector<int> > tmp1;
		vector<char*> tmp2;
		feats_vec.swap(tmp1);
		imgs_path.swap(tmp2);
	}
	return 0;
}
