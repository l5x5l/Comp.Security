#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <stdio.h>
#include <Windows.h>
#include <fstream>
using namespace std;

int main(int argc, char* argv[]) {
	//삽입할 글
	string insert_data = R"foo(
Lorem Ipsum is simply dummy text of the printing and
typesetting industry.Lorem Ipsum has been the
industry’s standard dummy text ever since the 1500s,
when an unknown printer took a galley of typeand
scrambled it to make a type specimen book.It has
survived not only five centuries, but also the leap into
electronic typesetting, remaining essentially unchanged.
It was popularized in the 1960s with the release of
Letraset sheets containing Lorem Ipsum passages, and
more recently with desktop publishing software like
Aldus PageMaker including versions of Lorem Ipsum.)foo";

	insert_data += char(127); //마지막 확인용도
	string temp_argv = "d"; //임시용
	unsigned char* output;

	//옵션을 입력했는지 여부를 확인
	if (argc != 2) {
		cout << "wrong input : <OPTION>" << endl;
		return 0;
	}

	FILE* f = NULL;
	FILE* of = NULL;
	const char* origin_path = "C:/cygwin64/home/sh103/CompSecurity/stego/origin.bmp"; //이후에 origin.bmp로 바꿀것
	const char* stego_path = "C:/cygwin64/home/sh103/CompSecurity/stego/stego.bmp"; //이후에 stego.bmp로 바꿀것
	unsigned char info[54];
	unsigned char outputinfo[54];

	f = fopen(origin_path, "rb");

	if (argv[1][0] == 'e' || argv[1][0] == 'd') {

		fread(info, sizeof(unsigned char), 54, f);
		//이미지의 너비와 높이
		int width = *(int*)&info[18];
		int height = *(int*)&info[22];
		//이미지의 bit per pixel
		int bpp = 24;
		//가로의 길이는 4의 배수로 맞춰야 하므로 1을 더했다
		int pmd_size = ((width + 1) * (height) * 3);

		if (argv[1][0] == 'e') { //'e' 옵션

			//outputinfo에 info를 복사(헤더 복사)
			for (int i = 0;i < 54;i++) {
				outputinfo[i] = info[i];
				if (i == 9) {
					cout << "test :: " << *(int*)&info[34] << "\n";
				}
			}
			of = fopen(stego_path, "wb");
			fwrite(info, 1, 54, of);

			int total_size = insert_data.length();
			cout << "total_size is " << total_size << "\n";
			unsigned char* data = new unsigned char[pmd_size];
			cout << "test :: " << pmd_size << "\n";
			output = new unsigned char[pmd_size];
			memset(output, 0, pmd_size);

			fread(data, sizeof(unsigned char), pmd_size, f);
			//삽입할 string의 크기(total_size)만큼 원본이미지에 string의 각 문자를 결합한 setgo 이미지 생성
			for (int i = 0;i < total_size;i++) { //여기
				unsigned char tempi = (unsigned char)insert_data[i];
				int times3 = i * 3;
				output[times3] = (unsigned char)((int)data[times3] + (((int)tempi) % 5));
				tempi = (unsigned char)(((int)tempi) / 5);
				output[times3 + 1] = (unsigned char)((int)data[times3 + 1] + (((int)tempi) % 5));
				tempi = (unsigned char)(((int)tempi) / 5);
				output[times3 + 2] = (unsigned char)((int)data[times3 + 2] + ((int)tempi));
			}
			//string의 크기 밖인 경우 그냥 픽셀값을 복사
			for (int i = sizeof(unsigned char) * total_size ; i * 3 < pmd_size;i++) {
				int times3 = i * 3;
				output[times3] = data[times3];
				output[times3 + 1] = data[times3 + 1];
				output[times3 + 2] = data[times3 + 2];
			}

			fwrite(output, 1, pmd_size, of);
			fclose(of);
		}
		else { //'d' 옵션
			of = fopen(stego_path, "rb");
			//stego파일의 픽셀부분만 다루기 위해 헤더를 일단 읽어옴
			fread(outputinfo, sizeof(unsigned char), 54, of);

			unsigned char* data = new unsigned char[pmd_size];
			output = new unsigned char[pmd_size];
			memset(data, 0, pmd_size);
			memset(output, 0, pmd_size);
			
			//원본 이미지와 stego이미지의 픽셀값들을 각각 data, output으로 읽어옴
			fread(data,  sizeof(unsigned char), pmd_size, f);
			fread(output,  sizeof(unsigned char), pmd_size, of);
			//숨겨진 메세지를 담을 string변수
			string stroutput = "";
			for (int i = 0; i * 3 < pmd_size;i++) {
				//한 픽셀당 3byte이므로 단위를 3씩 옮겨야 하지만(i+=3) 대신 i*3<pma_size와 times3 변수를 사용함
				int times3 = i * 3;
				int temp = 0;
				//저장한것의 역순으로 숨긴 문자열의 각 문자를 읽어옴
				temp += (int(output[times3]) - int(data[times3]));
				temp += (int(output[times3 + 1]) - int(data[times3 + 1])) * 5;
				temp += (int(output[times3 + 2]) - int(data[times3 + 2])) * 25;
				//문자열의 마지막('~')일 때 루프종료
				if (temp == 127) {
					break;
				}
				else {
					stroutput += (char)(temp);
				}
			}
			cout << stroutput << "\n";
		}
	}
	else { //옵션을 잘못 입력한 경우
		cout << "wrong options, option list is : e, d" << endl;
		return 0;
	}

	return 0;

}


//http://www.wearedev.net/71?PHPSESSID=8ea66fdc1e8f6d68b89dd7bffdc0b49d