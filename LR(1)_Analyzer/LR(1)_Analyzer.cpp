#include <iostream>
#include <string>
#include <cstring>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <map>
#include <set>
#include <vector>
#include <fstream>
#include <cstdio>
#include <stack>
#include <numeric>

#define debug cout

using namespace std;

vector< vector<char> > G;//����ķ���һ��vector��ά���飬һ�д��һ���ķ��������vectorǶ���÷�����ȥ����һ�¡�C++ ��vector��Ƕ��ʹ�á���ƪ����
map< char , set<char> > terminal;//����ս������FIRST���ϵ�ӳ��
map< char , set<char> > nonterminal;//��ŷ��ս������FIRST���ϵ�ӳ��

vector<string> token;//����ʷ����������token��������ת������д�Ķ����ķ�����ʶ��ķ��Ŵ������ս��������number��identifer��type��keyword��������������

void read_grammar2() {
	ifstream read("grammar2.txt");
	string content;
	vector<char> temp;//���ÿһ�г����Ƶ���������ַ����������ķ�G
	set<char> t;//���ڸ�map���ӽ�㣨���ս�����ս����
	while (getline(read, content)) {
		if (content[0] == '#') {
			if (content == "#endread") break;
			else continue;
		}
		nonterminal[content[0]] = t;
		//��������ʼ��������ķ�
		for (int i = 0; i < content.size(); i++) {
			char ch = content[i];//ch��ŵ�ǰ��content�ĵ�i���ַ�
			if (ch != '-' && ch != '>') {
				temp.push_back(ch);
			}
			if (i >= 3 && (content[i] < 'A' || content[i]>'Z')) terminal[content[i]] = t;
		}
		G.push_back(temp);
		temp.clear();
	}
	//�ع��ķ�,S'->S�����﷽���ʾ��S'��Z��ʾ
	temp.push_back('Z');
	nonterminal['Z'] = t;
	temp.push_back(G[0][0]);
	G.insert(G.begin(), temp);
}

//�鿴����Ķ����ķ�ȥ��ע�ͺ���ع��ķ�
void show_grammar2() {
	for (int i = 0; i < G.size(); i++) {
		for (int j = 0; j < G[i].size(); j++) {
			debug << G[i][j];
			if (j == 0) debug << "->";
			if (j == G[i].size() - 1) debug << endl;
		}
	}
}

//�Դʷ�������token�����д�������token��vector����
void deal_with_token() {
	ifstream read("token_output.txt");
	string content;
	while (getline(read, content)) {
		size_t flag1 , flag2 ;//flag1��flag2�ֱ��Ӧtoken��Ԫ���һ�����ź����һ�����ŵ�λ��
		for (flag1 = 0; flag1 < content.size(); flag1++) {
			if (content[flag1] == ',') break;
		}
		for (flag2 = content.size(); flag2 > flag1; flag2--) {
			if (content[flag2] == ',') break;
		}
		string str1, str2;
		for (int i = flag1 + 1; i < flag2; i++) {
			str1 += content[i];
		}
		for (int i = flag2 + 1; i < content.size(); i++) {
			str2 += content[i];
		}
		if (str1 == "return") {
			token.push_back("r");
			continue;
		}
		if (str1 == "int" || str1 == "void" || str1 == "bool" || str1 == "float" || str1 == "double" || str1 == "char") {
			token.push_back("t");
			continue;
		}
		if (str1 == "if") {
			token.push_back("j");
			continue;
		}
		if (str1 == "else") {
			token.push_back("k");
			continue;
		}
		if (str1 == "while") {
			token.push_back("w");
			continue;
		}
		if (str1 == "break") {
			token.push_back("b");
			continue;
		}
		if (str1 == "continue") {
			token.push_back("c");
			continue;
		}
		if (str2 == "limiter") {
			token.push_back(str1);
			continue;
		}
		if (str2 == "identifier" || str1=="main") {
			token.push_back("i");
			continue;
		}
		if (str2 == "number") {
			token.push_back("n");
			continue;
		}
		if (str2 == "operator") {
			if (str1 == ">" || str1 == "<" || str1 == ">=" || str1 == "<=" || str1 == "==" || str1 == "!=") {
				token.push_back("p");
				continue;
			}
			else {
				if (str1 == "=") token.push_back("=");
				else token.push_back("o=");
				continue;
			}
		}
	}
}

//�鿴tokenת����ķ��Ŵ�����
void show_token() {
	string temp;
	for (int i = 0; i < token.size(); i++) {
		temp += token[i];
	}
	debug << temp;
}

//���ݶ����ķ��õ��������ŵ�First����
void get_First() {
	for (auto &it : terminal) {
		it.second.insert(it.first);//�ս����first��������
	}
	bool ischanged = true;
	while (ischanged) {
		ischanged = false;
		for (auto& it : nonterminal) {
			//����ÿ�����ս������һ�������ķ�
			for (int i = 0; i < G.size(); i++) {
				//G��һ����ά���飬�б�ʾÿ���ķ����б�ʾÿ���ķ���ÿ���ַ�
				int size = it.second.size();
				if (G[i][0] == it.first) {
					//��������it������ָ����Ǹ�nonterminal
					auto iter = terminal.find(G[i][1]);//iter������ȥ�ս����map��Ѱ�ң��ҵ��򷵻ض�Ӧ��ַ�����򷵻�terminal.end()
					if (iter != terminal.end() || G[i][1] == '@') {
						//A->aֱ����һ���ս������� || A->@����յ����
						it.second.insert(G[i][1]);
						if (it.second.size() > size) ischanged = true;
					}
					else {
						//A->BC������£���FIRST(B)����FIRST(A)����B���Ƴ��գ����FIRST(C)����FIRST(A)
						bool can_to_empty = false;
						for (auto j = nonterminal[G[i][1]].begin(); j != nonterminal[G[i][1]].end(); j++) {
							if (*j == '@') can_to_empty = true;
						}
						if (!can_to_empty) {
							//B�����Ƴ��գ���FIRST(B)����FIRST(A)
							for (auto j = nonterminal[G[i][1]].begin(); j != nonterminal[G[i][1]].end(); j++) {
								it.second.insert(*j);
								if (it.second.size() > size) ischanged = true;
							}
						}
						else {
							//B���Ƴ��գ���{FIRST(B)-{@}}����{FIRST(C)}����FIRST(A)
							for (auto j = nonterminal[G[i][1]].begin(); j != nonterminal[G[i][1]].end(); j++) {
								if (*j != '@') it.second.insert(*j);
								if (it.second.size() > size) ischanged = true;
							}
							//Ȼ����������1�����ò���ʽֻ��A->B�����ռ���A
							if (G[i].size() == 2) {
								it.second.insert('@');
								if (it.second.size() > size) ischanged = true;
							}
							//2��A->BC��FIRST(C)����FIRST(A)����CҲ���ƿյ��������һ��ѭ���л���һ�δ��������ڵݹ��˼�룩
							else {
								for (auto j = nonterminal[G[i][2]].begin(); j != nonterminal[G[i][2]].end(); j++) {
									it.second.insert(*j);
									if (it.second.size() > size) ischanged = true;
								}
							}
						}
					}
				}
			}
		}
	}
}

void show_First() {
	for (auto& it : nonterminal) {
		//����nonterminal��map�������nonterminal.fist��Ҳ����һ���ս��
		cout << "FIRST(" << it.first << "):";
		int size = it.second.size();
		//����second��size()��С��ȡ�����е�ֵ�������ŵ���first����
		/*for (auto& iter1 : it.second)
			cout << iter1 << "	";*/
		for (auto iter = it.second.begin(); iter != it.second.end(); iter++) {
			cout << *iter << "	";
		}
		cout << endl;
	}
}

int main() {
	read_grammar2();
	//show_grammar2();
	deal_with_token();
	//show_token();
	get_First();
	show_First();
}