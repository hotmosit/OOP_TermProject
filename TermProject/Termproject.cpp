#define _CRT_SECURE_NO_WARNINGS

#include<fstream>
#include<iostream>
#include<string>
#include<vector>
#include<sstream>
#include<windows.h>
#include<sstream>

#define MAXBYTE 75

using namespace std;

struct Buffer {
	char buf[152] = "";
};

vector<Buffer> lines;
int pageNumber = 1;
int searchedLine = -1;
string consolMessage = "";


class Action {
public:
	virtual void doAction() = 0;

	void clear_buffer(Buffer& buffer) {
		for (int i = 0; i <= 151; i++) {
			buffer.buf[i] = '\0';
		}
	}

	void reWrite() {
		vector<Buffer> temp;
		string word, prev, sentance;
		int currentByte = 1;
		char buf[75] = "";
		Buffer buffer;

		word.clear();
		prev.clear();
		sentance.clear();

		temp.push_back(buffer);

		for (int i = 0; i < lines.size(); i++) {
			sentance = lines[i].buf;
			istringstream ss(sentance);

			while (getline(ss, word, ' ')) {
				if (!prev.empty()) {
					for (int i = 0; i < prev.length(); i++) {
						buffer.buf[i] = prev.at(i);
					}
					currentByte += prev.length();
					prev.clear();
				}

				word += ' ';
				currentByte += word.length();

				if (currentByte / MAXBYTE >= 1) {
					prev = word;
					currentByte = 1;
					temp.push_back(buffer);
					clear_buffer(buffer);
					word.clear();
					continue;
				}

				int  j = 0;

				for (int i = currentByte - word.length() - 1; i < currentByte - 1; i++) {
					buffer.buf[i] = word.at(j);
					j++;
				}

				word.clear();

			}
			word.clear();
			sentance.clear();
		}
		
		temp.push_back(buffer);

		

		lines.clear();
		lines.assign(temp.begin(), temp.end());

	}
};

class NextPage :public Action {
public:
	void doAction() override {
		// ��� �������� ������ ? ����ó�� �ʿ� 
		if (lines.size() / 20 < pageNumber) {
			consolMessage = "This is the last page!";
			return;
		}
		++pageNumber;
	}
};

class PrevPage : public Action {
public:
	void doAction() override {
		// ������ 1���� ������ ? ����ó�� �ʿ� 
		if (pageNumber - 1 < 1) {
			consolMessage = "This is the first page!";
			return;
		}
		--pageNumber;
	}
};

class Insert : public Action {
public:
	void i(int a, int b, string s) {
		line = (pageNumber - 1) * 20 + a;	// ������ ��ȣ�� �°� �ٽ� ����
		col = b;
		word = s;
	}
	void doAction() override {
		string temp = lines[line].buf;
		int wordCount = 0;
		int i;

		for (i = 0; i < temp.length(); i++) {
			if (temp.at(i) == ' ') {
				++wordCount;
			}

			if (wordCount + 1 == col) {
				break;
			}
		}

		temp.insert(i + 1, word + " ");
		strcpy(lines[line].buf, temp.c_str());

		// ���� ���� �� 75 ����Ʈ ũ�⸦ �ѱ�� ? 
		reWrite();

	}
public:
	int line, col;
	string word;
};

class Delete : public Action {
public:
	void d(int a, int b) {
		line = (pageNumber - 1) * 20 + a;	// �������� �´� ��ȣ�� �ٽ� ����
		col = b;
	}
	void doAction() override {
		string deletedWord;
		string temp = lines[line].buf;
		int wordCount = 0;
		int k;
		for (k = 0; k < temp.length(); k++) {
			if (temp.at(k) == ' ') {
				++wordCount;
			}
			if (wordCount + 1 == col) {
				break;
			}
		}

		for (int i = k + 1; lines[line].buf[i] != ' '; i++) {
			deletedWord += lines[line].buf[i];
		}

		temp.erase(k + 1, deletedWord.length() + 1);
		strcpy(lines[line].buf, temp.c_str());

		reWrite();
	}

public:
	int line, col;

};

class Change : public Action {
public:
	void c(string oldWord_, string newWord_) {
		oldWord = oldWord_;
		newWord = newWord_;
	}

	string replaceAll(string& str, const string& from, const string& to) {
		size_t start_pos = 0; //stringó������ �˻�
		while ((start_pos = str.find(from, start_pos)) != string::npos)  //from�� ã�� �� ���� ������
		{
			str.replace(start_pos, from.length(), to);
			start_pos += to.length(); // �ߺ��˻縦 ���ϰ� from.length() > to.length()�� ��츦 ���ؼ�
		}
		return str;
	}

	void doAction() {
		int lineStart = (pageNumber - 1) * 20 + 1;
		int lineEnd = lineStart + 19;
		string temp;

		for (int i = lineStart; i <= lineEnd; i++) {
			temp = lines[i].buf;
			temp = replaceAll(temp, oldWord, newWord);
			strcpy(lines[i].buf, temp.c_str());
		}

		reWrite();
	}
public:
	string oldWord, newWord;
};

class Search : public Action {
public:
	void s(string s_) {
		target = s_;
	}

	void doAction() {
		string s;

		for (int i = 0; i < lines.size(); i++) {
			s = lines[i].buf;
			
			if (s.find(target) <= 75) {
				searchedLine = i;
				break;
			}
		}

	}
public:
	string target;
};

class SaveQuit : public Action {
public:
	void doAction() {
		ofstream write("result.txt");

		for (int i = 0; i < lines.size(); i++) {
			string temp = lines[i].buf;
			write << temp;
		}
	}

};

class Term {
protected:
	Action* action;
	string filename;

public:
	Term(string filename_) : filename(filename_) {
		text_to_vector();
	}

	void show_page(int page) {
		int start;
		if (searchedLine != -1) {
			start = searchedLine;
		}
		else {
			start = 20 * (page - 1) + 1;
		}
		
		int end = start + 19;

		if (end > lines.size()) {
			end = lines.size() - 1;
		}

		int lineNum;

		for (int i = start; i <= end; i++) {
			lineNum = i - 20 * (page - 1);

			if (lineNum < 10) {
				cout << " " << lineNum << "| " << lines[i].buf << endl;
			}
			else {
				cout << lineNum << "| " << lines[i].buf << endl;

			}
		}

		searchedLine = -1;
	}

	string get_command() {
		string command;
		cin >> command;

		return command;
	}

	void show_interface() {
		string command;
		show_page(pageNumber);

		cout << "------------------------------------------------------------------------------------------" << endl;
		cout << "n:next page , p:previous page, i:insert, d:delete, c:change, s:search, t:save and quit " << endl;
		cout << "------------------------------------------------------------------------------------------" << endl;
		cout << "(consol message) " << consolMessage << endl;
		cout << "------------------------------------------------------------------------------------------" << endl;
		cout << "input:";
	}

	void setAction(Action* action_) {
		action = action_;
	}

	void doAction() {
		action->doAction();
	}

	void text_to_vector() {
		ifstream file;
		file.open(filename);
		string word, prev;
		int currentByte = 1;
		char buf[MAXBYTE] = "";
		Buffer buffer;

		lines.push_back(buffer);	// �� ���� ���� ����

		word.clear();
		prev.clear();

		while (file >> word) {

			if (!prev.empty()) {
				for (int i = 0; i < prev.length(); i++) {
					buffer.buf[i] = prev.at(i);
				}
				currentByte += prev.length();
				prev.clear();
			}

			word += ' ';
			currentByte += word.length();

			if (currentByte / MAXBYTE >= 1) {
				prev = word;
				currentByte = 1;
				lines.push_back(buffer);
				clear_buffer(buffer);
				word.clear();
				continue;
			}

			int  j = 0;

			for (int i = currentByte - word.length() - 1; i < currentByte - 1; i++) {
				buffer.buf[i] = word.at(j);
				j++;
			}
			word.clear();
		}
		lines.push_back(buffer);
		file.close();
	}

	void clear_buffer(Buffer& buffer) {
		for (int i = 0; i <= 151; i++) {
			buffer.buf[i] = '\0';
		}
	}
};


void string_data_handling(string command, vector<string>& splitedData) {
	string data = command.substr(2, command.length() - 3);
	string stringBuf;
	istringstream ss(data);

	while (getline(ss, stringBuf, ',')) {
		if (stringBuf.length() > MAXBYTE) {
			throw 0;
		}
		splitedData.push_back(stringBuf);
	}
}

void main() {
	string command;
	string data;	// ����� �Է°� ������ �����
	string stringBuf;
	vector<string> splitedData;
	Term* t = new Term("test.txt");
	Action* next = new NextPage;
	Action* prev = new PrevPage;
	Insert* insert = new Insert;
	Delete* del = new Delete;
	Search* search = new Search;
	Change* change = new Change;
	Action* saveQuit = new SaveQuit;
	int a, b;

	do {
		splitedData.clear();
		t->show_interface();
		command = t->get_command();

		consolMessage.clear();

		if (command.at(0) == 'n') {
			t->setAction(next);
			t->doAction();
			continue;
		}
		else if (command.at(0) == 'p') {
			t->setAction(prev);
			t->doAction();
		}
		else if (command.at(0) == 'i') {
			// insert ����
			try {
				string_data_handling(command, splitedData);
				stringstream(splitedData[0]) >> a;
				stringstream(splitedData[1]) >> b;
				insert->i(a, b, splitedData[2]);
				t->setAction(insert);
				t->doAction();
			}
			catch (int b) {
				consolMessage = "error";
			}
		}
		else if (command.at(0) == 'd') {
			// delete ����
			string_data_handling(command, splitedData);
			stringstream(splitedData[0]) >> a;
			stringstream(splitedData[1]) >> b;
			del->d(a, b);
			t->setAction(del);
			t->doAction();
		}
		else if (command.at(0) == 's') {
			// �ؽ�Ʈ ���� ó������ Ž��, ù��° ��� â�� ù��° ���ο� ��ġ�ϵ��� ���
			string_data_handling(command, splitedData);
			search->s(splitedData[0]);
			t->setAction(search);
			t->doAction();
		}
		else if (command.at(0) == 'c') {
			// �����ϰ� ���� �ܾ� ��� �ٸ� �ܾ�� ����
			string_data_handling(command, splitedData);
			change->c(splitedData[0], splitedData[1]);
			t->setAction(change);
			t->doAction();
		}
		else if (!command.compare("t")) {

			t->setAction(saveQuit);
			t->doAction();
		}
		else {
			consolMessage = "error: invalid command";
		}
		cout << "------------------------------------------------------------------------------------------" << endl;
		Sleep(100);
		system("cls");

	} while (command.compare("t"));
}