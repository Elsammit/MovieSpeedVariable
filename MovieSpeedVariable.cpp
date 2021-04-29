#include "opencv2/imgproc.hpp"
# include <opencv2/highgui.hpp>
# include <opencv2/videoio.hpp>
#include <iostream>
#include <thread>

#define FORWARD		1
#define REVERSAL 	2
#define SPEEDUP		30
#define SPEEDDOWN	40
#define SPUPSTAGE	10
#define SPDWNSTAGE	5

using namespace cv;

int flg = 1;	// キーボード入力判定用フラグ.

// キーボード入力スレッド.
void KeyInput(){
	char ipt[2];
	while(1){
		std::cin>>ipt;
		int buf = atoi(ipt);
		printf("buf:%d \n",buf);
		if(buf > 9){
			flg = buf + flg;
		}else{
			flg = buf;
		}
	}
}

// キーボード入力に応じた動画再生速度や巻き戻し判定処理.
int JudgeMovieProcess(int *counter, int *speed){
	int ret = 0;
	if(flg == FORWARD){			// 動画再生選択時.
		*counter+= *speed;
	}else if(flg == REVERSAL){	// 巻き戻し再生選択時.
		*counter-= *speed;
	}else if(flg/10 == SPEEDUP/10){	// 再生速度アップ.
		*speed += SPUPSTAGE;
        flg = flg % 10;				// 2桁目のフラグをリセットしないと速度が変化し続けるため.
	}else if(flg/10 == SPEEDDOWN/10){	// 再生速度ダウン.
		*speed -= SPDWNSTAGE;
		if(*speed < 1){ *speed = 1; }	// 再生速度が0以下にさせないため.
        flg = flg % 10;				// 2桁目のフラグをリセットしないと速度が変化し続けるため.
	}else{
		ret = -1;				// 誤った値が入力されていた場合にはエラーとして0以外をセット.	
	}
	return ret;	
}

int main(){
	std::thread th1(KeyInput);	// キー入力スレッド生成.

	// 動画読み取り.
	VideoCapture cap("Forest - 49981.mp4");
  	if(!cap.isOpened()) return -1;
	int max_frame = cap.get(CAP_PROP_FRAME_COUNT); 	//フレーム数.
	int fps = cap.get(CAP_PROP_FPS);				// FPS.
	printf("frame Num:%d, fps:%d \n",max_frame, fps);

	Mat frame;	// フレーム画像格納変数.
	namedWindow("edges", WINDOW_AUTOSIZE);
	int counter = 1;
	int speed = 1;
	for (;;) {
		if(JudgeMovieProcess(&counter, &speed) !=0){	// 動画再生速度・逆転判定.
			printf("Input Error \n");
		}
		cap.set(CAP_PROP_POS_FRAMES,counter);	// 読み込むフレーム数選択.
    	cap >> frame;							// 選択したフレーム数読み込み.

    	// ウィンドウを表示
    	imshow("edges", frame);
    	if(waitKey(30) >= 0) break;
  	}
}
