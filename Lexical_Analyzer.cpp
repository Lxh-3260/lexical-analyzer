#include <iostream>
#include <string>
#include <cstring>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <map>
#include <vector>
#include <fstream>
#include <cstdio>
#include <stack>

#define debug cout
using namespace std;

ifstream input;//�����ļ����������ķ���Դ����
ofstream output;//����ļ�����token��Ԫ�飩

/*����һ���ؼ������飬sacn()���룬ʶ��token��ʱ
	������ĸ����ĸ,���ȱ���������������token�Ƚ��ж��ǲ��ǹؼ���
		���ǹؼ��֣�break���keywordtoken
		�����ǣ�����DFA�ж����ǲ��ǺϷ��ı�ʶ��
*/
const char keyword[][10] = { 
	"int","float","double","char","bool" ,
	"if","else" ,"main" ,"void" ,"while" ,
	"break" ,"continue"  ,"long" ,"switch" ,
};

struct NFA_node {
	char node[100];

};

void create_NFA() {
	ifstream read("grammar3.txt");
	string content;
	while (read >> content) {
		//�������ķ����ļ�
		if (content[0] == '#') {
			if (content == "#endread") break;
			else continue;
		}
		//debug << content << endl;
	}
}

int main() {
	create_NFA();
	return 0;
}