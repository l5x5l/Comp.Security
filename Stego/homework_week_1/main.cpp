#include <iostream>
#include <string>
#include <crtdbg.h>
#include <Windows.h>
#include <fstream>
using namespace std;

int main(int argc, char* argv[]) {
	//������ ��
	string insert_data = R"foo(
Lorem Ipsum is simply dummy text of the printing and
typesetting industry.Lorem Ipsum has been the
industry��s standard dummy text ever since the 1500s,
when an unknown printer took a galley of typeand
scrambled it to make a type specimen book.It has
survived not only five centuries, but also the leap into
electronic typesetting, remaining essentially unchanged.
It was popularized in the 1960s with the release of
Letraset sheets containing Lorem Ipsum passages, and
more recently with desktop publishing software like
Aldus PageMaker including versions of Lorem Ipsum.)foo";

	insert_data += char(127); //������ Ȯ�ο뵵
	string temp_argv = "d"; //�ӽÿ�
	unsigned char* output;

	//�ɼ��� �Է��ߴ��� ���θ� Ȯ��
	if (argc != 2) {
		cout << "wrong input : <OPTION>" << endl;
		return 0;
	}

	FILE* f = NULL;
	FILE* of = NULL;
	const char* origin_path = "C:/Users/sh103/Documents/2020-2�б�/��ǻ�ͺ���/practice/homework_week_1/origin.bmp"; //���Ŀ� origin.bmp�� �ٲܰ�
	const char* stego_path = "C:/Users/sh103/Documents/2020-2�б�/��ǻ�ͺ���/practice/homework_week_1/stego.bmp"; //���Ŀ� stego.bmp�� �ٲܰ�
	unsigned char info[54];
	unsigned char outputinfo[54];


	if (0 != fopen_s(&f, origin_path, "rb")) {
		cout << "fali to read file" << endl;
		return 0;
	}
	// 527574

	if (argv[1] == "e" || argv[1] == "d") {

		fread_s(info, sizeof(unsigned char) * 54, sizeof(unsigned char), 54, f);
		//�̹����� �ʺ�� ����
		int width = *(int*)&info[18];
		int height = *(int*)&info[22];
		//�̹����� bit per pixel
		int bpp = 24;
		//������ ���̴� 4�� ����� ����� �ϹǷ� 1�� ���ߴ�
		int pmd_size = ((width + 1) * (height) * 3);

		if (argv[1] == "e") { //'e' �ɼ�

			//outputinfo�� info�� ����(��� ����)
			for (int i = 0;i < 54;i++) {
				outputinfo[i] = info[i];
				if (i == 9) {
					cout << "test :: " << *(int*)&info[34] << "\n";
				}
			}
			fopen_s(&of, stego_path, "wb");
			fwrite(info, 1, 54, of);

			int total_size = insert_data.length();
			cout << "total_size is " << total_size << "\n";
			unsigned char* data = new unsigned char[pmd_size];
			cout << "test :: " << pmd_size << "\n";
			output = new unsigned char[pmd_size];
			memset(output, 0, pmd_size);

			fread_s(data, sizeof(unsigned char) * pmd_size, sizeof(unsigned char), pmd_size, f);
			//������ string�� ũ��(total_size)��ŭ �����̹����� string�� �� ���ڸ� ������ setgo �̹��� ����
			for (int i = 0;i < total_size;i++) { //����
				unsigned char tempi = (unsigned char)insert_data[i];
				//cout << tempi <<" : " << ((int)tempi) <<"\n";
				int times3 = i * 3;
				//cout << "Origin => R : " << (int)data[times3] << " G : " << (int)data[times3 + 1] << " B : " << (int)data[times3 + 2] << "\n";
				output[times3] = (unsigned char)((int)data[times3] + (((int)tempi) % 5));
				tempi = (unsigned char)(((int)tempi) / 5);
				output[times3 + 1] = (unsigned char)((int)data[times3 + 1] + (((int)tempi) % 5));
				tempi = (unsigned char)(((int)tempi) / 5);
				output[times3 + 2] = (unsigned char)((int)data[times3 + 2] + ((int)tempi));
				//cout << "Atfer  => R : " << (int)output[times3] << " G : " << (int)output[times3 + 1] << " B : " << (int)output[times3 + 2] << "\n";
			}
			//string�� ũ�� ���� ��� �׳� �ȼ����� ����
			for (int i = sizeof(unsigned char) * total_size ; i * 3 < pmd_size;i++) {
				int times3 = i * 3;
				output[times3] = data[times3];
				output[times3 + 1] = data[times3 + 1];
				output[times3 + 2] = data[times3 + 2];
				//cout << "i : " << i << "\n";
				//cout << "Origin => R : " << (int)data[times3] << " G : " << (int)data[times3 + 1] << " B : " << (int)data[times3 + 2] << "\n";
				//cout << "Atfer  => R : " << (int)output[times3] << " G : " << (int)output[times3 + 1] << " B : " << (int)output[times3 + 2] << "\n";
			}

			fwrite(output, 1, pmd_size, of);
			fclose(of);
		}
		else { //'d' �ɼ�
			fopen_s(&of, stego_path, "rb");
			//stego������ �ȼ��κи� �ٷ�� ���� ����� �ϴ� �о��
			fread_s(outputinfo, sizeof(unsigned char) * 54, sizeof(unsigned char), 54, of);;

			unsigned char* data = new unsigned char[pmd_size];
			output = new unsigned char[pmd_size];
			memset(data, 0, pmd_size);
			memset(output, 0, pmd_size);
			
			//���� �̹����� stego�̹����� �ȼ������� ���� data, output���� �о��
			fread_s(data, sizeof(unsigned char) * pmd_size, sizeof(unsigned char), pmd_size, f);
			fread_s(output, sizeof(unsigned char) * pmd_size, sizeof(unsigned char), pmd_size, of);
			//������ �޼����� ���� string����
			string stroutput = "";
			for (int i = 0; i * 3 < pmd_size;i++) {
				//�� �ȼ��� 3byte�̹Ƿ� ������ 3�� �Űܾ� ������(i+=3) ��� i*3<pma_size�� times3 ������ �����
				int times3 = i * 3;
				int temp = 0;
				//�����Ѱ��� �������� ���� ���ڿ��� �� ���ڸ� �о��
				temp += (int(output[times3]) - int(data[times3]));
				temp += (int(output[times3 + 1]) - int(data[times3 + 1])) * 5;
				temp += (int(output[times3 + 2]) - int(data[times3 + 2])) * 25;
				//���ڿ��� ������('~')�� �� ��������
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
	else { //�ɼ��� �߸� �Է��� ���
		cout << "wrong options, option list is : e, d" << endl;
		return 0;
	}

	return 0;

}


//http://www.wearedev.net/71?PHPSESSID=8ea66fdc1e8f6d68b89dd7bffdc0b49d