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
#include <stdlib.h>

#define debug cout

using namespace std;

vector<vector<char>> G;//����ķ���һ��vector��ά���飬һ�д��һ���ķ��������vectorǶ���÷�����ȥ����һ�¡�C++ ��vector��Ƕ��ʹ�á���ƪ����
map<char, set<char>> terminal;//����ս������FIRST���ϵ�ӳ�䣬terminal.first��ʾmap��ǰһ�Ϊ�ս����Ӧ��char��terminal.second��ʾmap�ĺ�һ�Ϊ���ս����FIRST��
map<char, set<char>> nonterminal;//��ŷ��ս������FIRST���ϵ�ӳ�䣬nonterminal.first��ʾmap��ǰһ�Ϊ�ս����Ӧ��char��nonterminal.second��ʾmap�ĺ�һ�Ϊ���ս����FIRST��

vector<string> token;//����ʷ����������token��������ת������д�Ķ����ķ�����ʶ��ķ��Ŵ������ս��������number��identifer��type��keyword��������������

map< map<string, char>, string> table; //LR������
ofstream output;

//��Ŀ��
struct Project {
	vector<vector<char>> project;//��Ŀ��I����ά���飬�б�ʾ����Ŀ���ж����в���ʽ���б�ʾ����ʽ�ĸ����ַ�
	vector<set<char>> search_forward;//��Ŀ��I��ÿ������ʽ��Ӧ����ǰ�������ż�������ÿ���ս����һ������ʽ����ǰ�������ż�ֻ�ܳ���һ�Σ���set���ϸ����������ݽṹ
	map<char, int> go;//ÿ����Ŀ����һ��char�ַ���Ҫת�Ƶ�����һ����Ŀ���
};
vector<Project> clousure;//clousure��ʾ������Ŀ���ıհ�

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
	//debugʱ�����ifstream���ļ�������vector out of range�����ش��󣬹ʼ���exit(0)����û�����ļ�ʱֱ���˳�
	if (G.empty()) exit(0);
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

//�õ�tokenת����ķ��Ŵ�����
string get_token_string() {
	string temp;
	for (int i = 0; i < token.size(); i++) {
		temp += token[i];
	}
	return temp;
}

//���ݶ����ķ��õ��������ŵ�First����
void get_First() {
	for (auto &it : terminal) {
		it.second.insert(it.first);//�ս����first��������
	}
	bool ischanged = true;//ÿ��insert�����ῴ���ս����FIRST.size()�Ƿ�ı䣬ֻҪ�ı��ˣ���Ҫһֱѭ������һ��
	while (ischanged) {
		ischanged = false;
		for (auto& it : nonterminal) {
			//����ÿ�����ս������һ�������ķ���it.firstΪ��ǰ���ս����it.secondΪ���ս����First����
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
							//1. {FIRST(B)-{@}}
							for (auto j = nonterminal[G[i][1]].begin(); j != nonterminal[G[i][1]].end(); j++) {
								if (*j != '@') it.second.insert(*j);
								if (it.second.size() > size) ischanged = true;
							}
							//2. Ȼ����������
							//2.1 ���ò���ʽֻ��A->B����CΪ�գ�{FIRST(C)}='@'�����ռ���A
							if (G[i].size() == 2) {
								it.second.insert('@');
								if (it.second.size() > size) ischanged = true;
							}
							//2.2 A->BC��FIRST(C)����FIRST(A)����CҲ���ƿյ��������һ��ѭ���л���һ�δ��������ڵݹ��˼�룩
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

//debugʱչʾFIRST��������֤����ȷ��
void show_First() {
	for (auto& it : nonterminal) {
		//����nonterminal��map�������nonterminal.fist��Ҳ����һ���ս��
		debug << "FIRST(" << it.first << "):";
		int size = it.second.size();
		//����second��size()��С��ȡ�����е�ֵ�������ŵ���first����
		/*for (auto& iter1 : it.second)
			cout << iter1 << "	";*/
		for (auto iter = it.second.begin(); iter != it.second.end(); iter++) {
			debug << *iter << "	";
		}
		debug << endl;
	}
}

//����G�ж�ȡ�Ķ����ķ��õ���Ŀ���壬ע��ע���е���Ŀ��==�հ�����Ŀ==����ʽ����Ŀ����Ҳ�������бհ��ļ��ϣ�дע�͵�ʱ������ˣ�ϣ�������ܿ���
void get_Clousure() {
	int i = 0;//ÿ����Ŀ���ı��
	Project p;
	clousure.push_back(p);
	while (true) {
		if (i == clousure.size()) break;//û���µıհ�����ʱ������ѭ��
		if (i == 0) {
			//�ȳ�ʼ��I0��Ŀ���ĵ�һ������ʽS��->��S,#
			vector<char> temp1(G[0]);//temp1���²���ʽ
			temp1.insert(temp1.begin() + 1, ' ');//�ÿո�space���桤����Ϊ����Բ���ַ����޷����ַ���ASCII��ʶ�𣬺������е���Ŀ���ͱհ��еĿո��Ǳ�ʾ��
			clousure[0].project.push_back(temp1);
			set<char> temp2;//temp2������ǰ�������ż�
			temp2.insert('#');
			clousure[0].search_forward.push_back(temp2);
		}

		//��ÿ����Ŀ����һ������ʽ��չ�����ıհ���Ŀ
		for (int j = 0; j < clousure[i].project.size(); j++) {
			//����������Ŀ����jΪÿ����Ŀ���еĵ�j����Ŀ
			for (int k = 0; k < clousure[i].project[j].size(); k++) {
				//��������Ŀ�����ҵ�����λ�ã�kΪÿ����Ŀ�и����ս�����ս���ı��
				if (clousure[i].project[j][k] == ' ') {
					//�ҵ��ˡ���λ�ã���ʼ�ӡ��ĺ���һ���ս�������µĲ���ʽ
					if (k == clousure[i].project[j].size() - 1) {
						//���ڲ���ʽ����󣬱���˵S'->S��,#  ��ô��������ʽ�����Ƴ��µĲ���ʽ
						//�ʸ���Ŀ���ġ�����Ҫ����break������һ����Ŀ��
						break;
					}
					for (int l = 0; l < G.size(); l++) {
						//���ڲ���ʽ���м�����������˵A->����B��,a
						//�������еĶ����ķ�G�����ҡ�����ķ��ս�����еĲ���ʽ�������µĲ���ʽ������Ŀ��
						if (G[l][0] == clousure[i].project[j][k + 1]) {
							vector<char> temp1(G[l]);
							temp1.insert(temp1.begin() + 1, ' ');
							//�����µĲ���ʽ����ֱ�Ӳ��뵽��ǰ��project��
							//���жϸ��µ���Ŀ�Ƿ�����ڵ�ǰ��Ŀ�������������¼�±�ţ��������������
							int project_is_exist = 0;
							for (int m = 0; m < clousure[i].project.size(); m++) {
								if (temp1 == clousure[i].project[m]) {
									project_is_exist = m;
									break;
								}
							}
							if(project_is_exist == 0) clousure[i].project.push_back(temp1);
							//�����￪ʼ������ǰ�������ż�
							set<char> temp2;//����������ʱ����ǰ�������ż�����������push_back��
							/*
							�������ʽ��A->����B��,a  ������ǰ�������ż���������FIRST(��a)����4�������
							1.�ò���ʽû�Ц£�����ʽΪA->����B,a����ǰ�������ż�Ϊ{a}
							2.�µĵ�һ���ַ�Ϊ�ս��{b}������ʽΪA->����Bb,a����ô��ǰ�������ż�Ϊ���ս��{b}
							
							3.�µĵ�һ���ַ�Ϊ���ս��C������ʽΪA->����BC,a���Ҹ÷��ս�����Ƴ���,��ǰ�������ż�ΪFIRST(C)-{'@'}����{��a}�����Ц�Ϊȥ���µĵ�һ������C����ַ���
							4.�µĵ�һ���ַ�Ϊ���ս��C������ʽΪA->����BC,a���Ҹ÷��ս�������Ƴ���,��ǰ�������ż�ΪFIRST(C)
							*/
							bool deduce_empty = true;//�жϦµĵ�һ���ַ�C�ܲ����Ƴ���
							int n = 0;//n��¼ȥ���µĵ�n���ַ�
							while (deduce_empty) {
								deduce_empty = false;
								if (k + n + 1 == clousure[i].project[j].size() - 1) {
									//��һ�������Ϊ��
									for (auto it : clousure[i].search_forward[j]) temp2.insert(it);
								}
								else if (terminal.find(clousure[i].project[j][k + n + 2]) != terminal.end()) {
									//�ڶ�����������ַ�Ϊ�ս��
									temp2.insert(clousure[i].project[j][k + n + 2]);
								}
								else {
									//��������������µĵ�һ���ַ�Ϊ���ս��
									//�ҵ�C��FIRST�������е��ս��
									set<char> temp_nonter(nonterminal.find(clousure[i].project[j][k + n + 2])->second);//ע��find�ҵ�����һ����ַ�����������÷�������һ��ֵ�����Բ�����.second������->second
									for (auto it : temp_nonter) {
										if (it == '@') {
											//�����һ���ַ����Ƴ���
											deduce_empty = true;//�ٽ���һ��ѭ������FIRST(��a)������
											n++;
										}
										else {
											//�����һ���ַ������Ƴ���
											temp2.insert(it);
										}
									}
								}
							}
							/*
							��������Ŀclousure[i].project[j]����ǰ�������ż��ݴ���set<char> temp2��
							����ǰ�������ż��Ĵ����Ϊ2�֣�
							1.temp1ԭ�Ȳ��ڸ���Ŀ���У�����ԭ����ʽ���沿�ֵ�FIRST���ϣ���ȷ������ǰ�������ż�
							2.temp1ԭ������Ŀ���У�����ԭ��ʽ���õ��µ���ǰ�������ż�����ԭ�Ȳ���ʽ����ǰ�������ż���
							*/
							if (project_is_exist == 0) {
								//��һ�������temp1������Ŀ����
								clousure[i].search_forward.push_back(temp2);
							}
							else {
								//��һ�������temp1����Ŀ����
								for (auto it : temp2) {
									clousure[i].search_forward[project_is_exist].insert(it);
								}
							}
						}
					}
					
					//�Ѿ�����������Ŀ���ġ��ˣ�break������һ����Ŀ��
					break;
				}
			}
		}

		//�жϸñհ��Ƿ����Ѿ����ֵıհ�,������հ�֮���໥ת���ı�
		for (int j = 0; j < clousure[i].project.size(); j++) {
			//��������Ŀ��
			for (int k = 0; k < clousure[i].project[j].size(); k++) {
				//ɨ��ÿ������ʽ���ҵ�����λ��
				if (clousure[i].project[j][k] == ' ') {
					//debugʱ����project���棬��д��һ��[j]������vector�������ôС�Ĵ���debug�˰���
					//��Ϊ�����ǶԵ�i����Ŀ�����հ����ĵ�j����Ŀ������ʽ�����б������ҵ�������λ�ã�������ȻҪ��project[j].size()
					if (k == clousure[i].project[j].size() - 1) {
						//������ڲ���ʽ�����ô�������ʽ�������µıհ���break��������һ������ʽ
						break;
					}
					//���򡤲��ڲ���ʽ������򽫵����һλ
					//Ȼ�����հ�֮��ıߣ������µıհ�
					vector<char> new_clousure_project(clousure[i].project[j]);//�µĲ���ʽ����ԭ����ʽ�Ƴ����ʳ�ʼ��ΪԴ����ʽclousure[i].project[j]��
					new_clousure_project[k] = new_clousure_project[k + 1];//new_clousure_project[k]��ŵ��Ǵ������ַ�
					new_clousure_project[k + 1] = ' ';
					char ch = new_clousure_project[k];//ch��Ŵ������ַ�(����Ƕ��̫���ˣ���new_clousure_project[k]�ܻ���)
					set<char> new_clousure_search_forward(clousure[i].search_forward[j]);//�µ���ǰ�������ż�����ԭ����ʽ��ǰ�������ż�һ����
					bool is_new_clousure = false;//�ж������ɵ���Ŀ�Ƿ����µıհ��������ж�Ҫ��Ҫ�����±հ���
					for (int m = 0; m < clousure.size(); m++) {
						//�������еıհ��������ñհ��Ƿ��Ѿ����ֹ�����Ŀ���������ǰ�������ż�Ҳ��ȣ�
						for (int n = 0; n < clousure[m].project.size(); n++) {
							//����ÿ���հ���������Ŀ��
							is_new_clousure = false;
							if (new_clousure_project == clousure[m].project[n]) {
								//������±�Ҳ��debug�˰��죬Ӧ���Ǹ���һ������ʽ����ǰ�������ż�size�Աȣ����Ƿ�һ������Ϊ�������һ������ʽ��project��search_forward��һ��������֪���հ���ȻҲһ��
								//�����Ǹ�������Ŀ���������ż��Ĵ�С�ȣ���һ��д��ʱ��search_forward������д�˸�[0]������debug�˰����˷���ʱ�䣬д������Ӧ����д˼·α�룬�����ż�
								if (clousure[m].search_forward[0].size() != new_clousure_search_forward.size()) {
									is_new_clousure = true;
									break;
								}
								auto it1 = clousure[m].search_forward[0].begin();
								for (auto it2 : new_clousure_search_forward) {
									if (it2 != *it1) {
										is_new_clousure = true;
										break;
									}
									/*if (it1 == clousure[m].search_forward[0].end()) {
										is_new_clousure = false;
										break;
									}*/
									it1++;
								}
								if (is_new_clousure == false) {
									clousure[i].go[ch] = m;
									break;
								}
							}
							else is_new_clousure = true;

							if (is_new_clousure == false) break;
						}
						if (is_new_clousure == false) break;
					}
					/*
					���������������һ�ε�һ��T��go���������������ڶ���Ҫ�ѵڶ��еĲ���ʽ�����һ�ε�go����ָ����Ǹ��հ�
					E->��T��)/+
					T->��T*F��)/+/* 
					*/
					if (clousure[i].go.count(new_clousure_project[k]) != 0 && is_new_clousure) {
						clousure[clousure[i].go[ch]].project.push_back(new_clousure_project);
						clousure[clousure[i].go[ch]].search_forward.push_back(new_clousure_search_forward);
						break;
					}
					//�����û���ֹ����µıհ�������ñհ�
					if (is_new_clousure == true) {
						Project new_clousure;
						new_clousure.project.push_back(new_clousure_project);
						new_clousure.search_forward.push_back(new_clousure_search_forward);
						clousure.push_back(new_clousure);
						//go��һ��char��int��ӳ�䣬��ʾ�����Ӧ��char���ñհ�����ת���ĸ��հ���
						//����A->����BC,a����ô����������ž���B��Ҳ���Ƕ�Ӧnew_clousure_project[k]
						//��������ķ�����new_clousure_project[k]���µıհ���clousure.size()-1
						clousure[i].go[ch] = clousure.size() - 1;
					}
				}
			}
		}
		
		i++;
	}
}

//debugʱչʾ��Ŀ��������֮���໥ת���ĺ���������֤����ȷ��
void show_Clousure() {
	for (int i = 0; i < clousure.size(); i++) {
		printf("I%d:\n", i);
		for (int j = 0; j < clousure[i].project.size(); j++) {
			for (int k = 0; k < clousure[i].project[j].size(); k++) {
				if (clousure[i].project[j][k] != ' ')cout << clousure[i].project[j][k];
				else cout << "��";
				if (k == 0) cout << "->";
			}
			cout << ',';
			int count = 0;
			for (auto it = clousure[i].search_forward[j].begin(); it != clousure[i].search_forward[j].end(); it++) {
				cout << *it;
				
				if (count != clousure[i].search_forward[j].size() - 1) cout << '/';
				count++;
			}
			cout << endl;
		}
		for (auto& it : clousure[i].go) {
			cout <<"input char: " << it.first <<" to clousure I" << it.second << endl;
		}
		cout << endl;
	}
}

//�õ���Ŀ���󣬼���LR(1)�ķ���ACTION GOTO��
bool get_LR1Table() {
	/*
	��Ϊ���������˼�뷽�����廪��ѧ�̲�p146��
	1. S��->S��,#      ����Ik����ACTION����[k,#]Ϊacc
	2. A->�¡�,a       ����Ik����ACTION����[k,a]Ϊrj����ʾ�õ�j������ʽ��Լ
	3. A->����a��,b    ����Ik����Ik�ƽ�aת�Ƶ�Ij����ACTION����[k,a]ΪSj����ʾ���������a��״̬j�ֱ������ķ�����ջ��״̬����ջ
	4. A->����B��,a    ����Ik����Ik�ƽ�Bת�Ƶ�Ij����GOTO����[k,B]Ϊj����ʾ�õ�ǰ�ķ�����ջ��ΪA��״̬ջ��Ϊj
	*/
	for (int i = 0; i < clousure.size(); i++) {
		for (int j = 0; j < clousure[i].project.size(); j++) {
			for (int k = 0; k < clousure[i].project[j].size(); k++) {
				if (clousure[i].project[j][k] == ' ') {
					//�ҵ�ÿ���ķ��ġ�
					if (k == clousure[i].project[j].size() - 1) {
						//�������ķ������Ҫ��ʽ�ӽ��й�Լ
						//��Ϊ1 2�������
						if (clousure[i].project[j][0] == 'Z') {
							//��Ӧ������ע�͵ĵ�һ�����S��->S��,#
							map<string, char> m;
							string state = to_string(i);
							m[state] = '#';
							if (table.find(m) != table.end() && table[m] != "acc") {
								cout << "error";
								return false;
							}
							else {
								table[m] = "acc";
							}
						}
						else {
							//��Ӧ������ע�͵ĵڶ������A->�¡�,a
							int G_num = 0;
							for (int x = 0; x < G.size(); x++) {
								vector<char> y(clousure[i].project[j]);
								y.pop_back();//clousure[i].project[j]�ȶ����ķ���������һ��������vector��Ϊspace�ո�pop���������ֱ�ӱȽ�
								if (y == G[x]) {
									G_num = x;
									break;
								}
							}
							map<string, char> m;
							string state = to_string(i);
							for (auto it : clousure[i].search_forward[j]) {
								m[state] = it;
								if (table.find(m) != table.end() && table[m] != "r" + to_string(G_num)) {
									//debug << i;
									//debug�����з���table�һ��״̬��һ���ַ����룬����������ͬ�Ĺ�Լ�������˹�Լ��Լ��ͻ
									//���i�ҵ��г�ͻ����Ŀ��,����������ʽK��ԭ��ע�͵��й�K���ķ�����д���ķ��ָֻ�����LR1�ķ�
									cout << "error";
									return false;
								}
								else {
									table[m] = "r" + to_string(G_num);
								}
							}
						}
					}
					else {
						//��������ƽ�
						//��Ϊ3 4�������
						char next_ch = clousure[i].project[j][k + 1];//������һ���ַ����ж����ս�����Ƿ��ս������Ϊ�������
						if (terminal.find(next_ch) != terminal.end()) {
							//A->����a��,b����Ӧ����ĵ��������
							map<string, char> m;
							m[to_string(i)] = next_ch;
							if (table.find(m) != table.end() && table[m] != "S" + to_string(clousure[i].go[next_ch])) {
								cout << "error";
								return false;
							}
							else {
								table[m] = "S"+to_string(clousure[i].go[next_ch]);
							}
						}
						else {
							//A->����B��,a����Ӧ����ĵ��������
							map<string, char> m;
							m[to_string(i)] = next_ch;
							if (table.find(m) != table.end() && table[m] != to_string(clousure[i].go[next_ch])) {
								cout << "error";
								return false;
							}
							else {
								table[m] = to_string(clousure[i].go[next_ch]);
							}
						}
					}
					
					//����������������Ƿ�����������Ŀ��break������һ����Ŀ
					break;
				}
			}
		}
	}
	return true;
}

//debug������չʾLR1��Ŀ��
void show_LR1Table() {
	for (int i = -1; i < (int)clousure.size(); i++) {
		if (i == -1) {
			//���ACTION GOTO�ĵ�һ�С�״̬��
			cout << std::left << setw(5) << "״̬";
		}
		else {
			//���ACTION GOTO�ĵ�һ��״̬��
			cout << std::left << setw(5) << i;
		}
		for (auto& it : terminal) {
			//�����ս����ACTION GOTO��
			if (i == -1) {
				//i==-1ʱ���ACTION GOTO�ĵ�һ���ս������
				cout << std::left << setw(4) << it.first;
			}
			else {
				//i!=-1ʱ���ACTION GOTO���ս�����ֵ�TABLE
				map<string, char> m;
				m[to_string(i)] = it.first;
				cout << std::left << setw(4) << table[m];
			}
		}
		//����#��Table
		if (i == -1) {
			cout << std::left << setw(4) << '#';
		}
		else {
			map<string, char> m;
			m[to_string(i)] = '#';
			cout << std::left << setw(4) << table[m];
		}
		for (auto& it : nonterminal) {
			//������ս����ACTION GOTO��
			if (it.first == 'Z') {
				continue;
			}
			if (i == -1) {
				//i==-1ʱ���ACTION GOTO�ĵ�һ�з��ս������
				cout << std::left << setw(4) << it.first;
			}
			else {
				//i!=-1ʱ���ACTION GOTO����ս�����ֵ�TABLE
				map<string, char> m;
				m[to_string(i)] = it.first;
				cout << std::left << setw(4) << table[m];
			}
		}
		cout << endl;
	}
}

//�������token������LR1��ACTION GOTO ����з���
bool LR1_Analyze() {
	cout << "����\t\t" << "״̬ջ\t\t\t\t" << "����ջ\t\t\t\t" << "�����봮\t\t\t\t" << "ACTION GO" << endl;
	output << "����\t\t" << "״̬ջ\t\t\t\t" << "����ջ\t\t\t\t" << "�����봮\t\t\t\t" << "ACTION GO" << endl;
	vector<string> status_stack;//״̬ջ
	status_stack.push_back("0");//��ʼ��״̬ջ
	vector<char> symbol_stack;//����ջ
	symbol_stack.push_back('#');//��ʼ������ջ
	string str_token = get_token_string();//�����봮
	str_token += "#";//��ʼ�������봮
	int steps = 1;
	while (true) {
		cout << steps++ << "\t\t";
		output << steps++ << "\t\t";
		for (int i = 0; i < status_stack.size(); i++) {
			cout << status_stack[i];
			output << status_stack[i];
		}
		cout << "\t\t\t\t";
		output << "\t\t\t\t";
		for (int i = 0; i < symbol_stack.size(); i++) {
			cout << symbol_stack[i];
			output << symbol_stack[i];
		}
		cout << "\t\t\t\t";
		output << "\t\t\t\t";
		for (int i = 0; i < str_token.size(); i++) {
			cout << str_token[i];
			output << str_token[i];
		}
		cout << "\t\t\t\t";
		output << "\t\t\t\t";
		string status_top = status_stack[status_stack.size() - 1];//״̬ջ���ұ��Ǹ�״̬
		char ch_head = str_token[0];//�����봮������Ǹ��ַ�
		map<string, char> m;
		m[status_top] = ch_head;//����ӳ�䣬��ACTION GOTO�����Ҷ�Ӧλ�õ��ַ�����acc ����rk ����Sk
		if (table[m] == "acc") {
			//�����Ӧλ�õ��ַ���Ϊacc�����ʾ�����ɹ�
			cout << "YES!" << endl;
			output << "YES!" << endl;
			return true;
		}
		else if (table[m][0] == 'S') {
			//�����Ӧλ�õ��ַ�����һλΪS�������ֵΪSk�����ʾҪ���������������״̬ջ�м���״̬k������������Ŵ�����һλ����ջ
			string temp;
			for (int i = 1; i < table[m].size(); i++) temp += table[m][i];
			status_stack.push_back(temp);
			symbol_stack.push_back(str_token[0]);
			str_token = str_token.substr(1);
			cout << table[m] << "����״̬" << atoi(temp.c_str()) << "ѹ��״̬ջ" << endl;
			output << table[m] << "����״̬" << atoi(temp.c_str()) << "ѹ��״̬ջ" << endl;
		}
		else if (table[m][0] == 'r') {
			/*�����Ӧλ�õ��ַ�����һλΪr�������ֵΪrk�����ʾҪ���й�Լ�������������£�
			1.�ҵ���Ӧ���ķ����ķ��ұ��м����ַ�������ջ״̬ջ��Ҫpop����
			2.���ͬʱ������ջpush_back�����ķ�������ַ�����ʱ��״̬ջ��������ߵ��ַ���ת��GOTO���е��Ǹ�״̬��������״̬ѹ��ջ��
			 */
			string temp;
			for (int i = 1; i < table[m].size(); i++) temp += table[m][i];
			int pop_frequency = G[atoi(temp.c_str())].size()-1;
			while (pop_frequency--) {
				status_stack.pop_back();
				symbol_stack.pop_back();
			}
			symbol_stack.push_back(G[atoi(temp.c_str())][0]);
			char symbol_temp = G[atoi(temp.c_str())][0];
			string status_temp = status_stack[status_stack.size() - 1];
			map<string, char> m_temp;
			m_temp[status_temp] = symbol_temp;
			status_stack.push_back(table[m_temp]);
			cout << table[m] << "����";
			output << table[m] << "����";
			for (int i = 0; i < G[atoi(temp.c_str())].size(); i++) {
				cout << G[atoi(temp.c_str())][i];
				output << G[atoi(temp.c_str())][i];
				if (i == 0) {
					cout << "->";
					output << "->";
				}
			}
			cout << "��Լ������״̬" << table[m_temp] << "��ջ" << endl;
			output << "��Լ������״̬" << table[m_temp] << "��ջ" << endl;
		}
		else {
			//�����Ǹ�λ��Ϊ�գ������������
			//1.ȥ@��һ���ң�����Sk��k��״̬ջ
			//2.��ʾ��������
			string temp_status = status_stack[status_stack.size() - 1];
			map<string, char> temp_m;
			temp_m[temp_status] = '@';
			if (table.find(temp_m) != table.end()) {
				string temp;
				for (int i = 1; i < table[temp_m].size(); i++) temp += table[temp_m][i];
				status_stack.push_back(temp);
				symbol_stack.push_back('@');
				continue;
			}
			else {
				cout << "NO!" << endl;
				output << "NO!" << endl;
				return false;
			}
		}
	}
}

int main() {
	read_grammar2();
	//show_grammar2();
	deal_with_token();
	//show_token();
	//cout << endl;
	get_First();
	//show_First();
	get_Clousure();
	//show_Clousure();
	get_LR1Table();
	//show_LR1Table();
	output.open("LR(1)Analyzer_Program.txt");
	LR1_Analyze();
	output.close();
}