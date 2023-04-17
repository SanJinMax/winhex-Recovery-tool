#include<iostream>
#include<vector>
#include<fstream>
#include<iomanip>
#include "Statement.h"
#include<Windows.h>
using namespace std;

// FAT32 实现
class FAT32 {
public:
	uint64_t dbr, fat1, fat2, root, first, Capacity;
	uint16_t Cu;
	uint32_t fat;
};

void Template_FAT32() {
	cout <<
		"-----------------FAT32 DBR 模板------------------" << endl <<
		"               固定值                  簇  保留" << endl <<
		"EB 58 90 4D 53 44 4F 53 35 2E 30 00 02 10 3A 10" << endl <<
		"  FAT表数           固定值           相对扇区   " << endl <<
		"02 00 00 00 00 F8 00 00 3F 00 FF 00 00 80 00 00" << endl <<
		"分区总容量  FAT容量                  root起始簇 " << endl <<
		"00 18 3F 01 E3 27 00 00 00 00 00 00 02 00 00 00" << endl <<
		"01 00 06 00 00 00 00 00 00 00 00 00 00 00 00 00" << endl <<
		"80 00 09 00 00 00 00 4E 4F 20 4E 41 4D 45 20 20" << endl <<
		"20 20 46 41 54 33 32 20 20 20" << endl <<
		"-------------------------------------------------" << endl;
}

void Traverse_FAT32(FAT32& fat32) {
	cout << "请输入DBR所在扇区：";
	cin >> fat32.dbr;
	cout << "请输入FAT1所在扇区 (F8 FF FF 0F)：";
	cin >> fat32.fat1;
	cout << "保留扇区数为：" << fat32.fat1 - fat32.dbr << endl; // FAT1位置 - DBR位置 = 保留扇区
	cout << "请输入FAT2所在扇区 (F8 FF FF 0F)：";
	cin >> fat32.fat2;
	fat32.fat = (uint32_t)(fat32.fat2 - fat32.fat1); // FAT2位置 - FAT1位置 = FAT表容量
	cout << "FAT表的容量为：" << fat32.fat << endl;
	fat32.root = fat32.fat2 + fat32.fat; // FAT2 + FAT容量 = 根目录位置
	cout << "根目录（root）所在扇区为：" << fat32.root << endl;
	cout << "请输入root之后第一个文件夹或文件所在扇区：";
	cin >> fat32.first;
	fat32.Cu = (uint16_t)(fat32.first - fat32.root); // 第一个文件所在位置 - 根目录位置 = 簇
	cout << "每簇扇区数为：" << fat32.Cu << endl; // 计算簇
	fat32.Capacity = (uint64_t)(fat32.fat) * 512 / 4 * fat32.Cu; // FAT表容量 * 512 / 4 * 簇 = 总容量
	cout << "分区总扇区数(总容量)约为：" << fat32.Capacity << endl;
	cout << "------------------------------------------------------" << endl;
} // 遍历FAT32

void Correct_FAT32(uint16_t Cu) {
	if (Cu == 0) {
		cout << "请输入簇扇区数：";
		cin >> Cu;
	}
	cout << "输入文件位置：";
	uint32_t swap;
	cin >> hex >> swap;
	cout.fill('0');
	swap = swapInt32(swap); // 矫正数值
	printf("转换后的值为：%08X\n", swap);
	cout << "文件起始簇为：" << swap << endl;
	cout << "从根目录跳转扇区数：" << (swap - 2) * Cu << endl;
	cout << "------------------------------------------------------" << endl;
}

uint32_t swapInt32(uint32_t val) { // 00 00 29 0d
	return (val & 0x00FF0000) << 8 | (val & 0xFF000000) >> 8 |
		(val & 0x00FF) << 8 | (val & 0xFF00) >> 8;
} // 返回矫正文件位置数值
// FAT32 实现完毕

// NTFS 实现
class NTFS {
public:
	uint64_t dbr, mftMir, mft, mftStartCu, mftCu, bitmapByte, temp;
	uint16_t Cu;
	uint32_t Capacity;
};

void Template_NTFS() {
	cout <<
		"-----------------NTFS DBR 模板-------------------" << endl <<
		"               固定值                  簇 -----" << endl <<
		"EB 52 90 4E 54 46 53 20 20 20 20 00 02 08 00 00" << endl <<
		"                    固定值            相对扇区  " << endl <<
		"00 00 00 00 00 F8 00 00 3F 00 FF 00 00 80 00 00" << endl <<
		"         固定值         分区总容量  -----------" << endl <<
		"00 00 00 00 80 00 80 00 FF E7 3F 01 00 00 00 00" << endl <<
		" MFT起始簇  ------------ MFT备份起始簇---------  " << endl <<
		"00 00 0C 00 00 00 00 00 02 00 00 00 00 00 00 00" << endl <<
		"F6 00 00 00 01 00 00 00" << endl
		<< "-------------------------------------------------" << endl;
}

void Traverse_NTFS(NTFS& ntfs) {
	char ch;
	cout << "请输入DBR所在扇区 (EB 52 90)：";
	cin >> ntfs.dbr;
	cout << "是否存在MFTMir (Y/N)：";
	cin >> ch;
	if (ch == 'y' || ch == 'Y') {
		cout << "请输入MFTMir所在扇区 (46 49 4C 45)：";
		cin >> ntfs.mftMir;
		ntfs.Cu = (uint16_t)(ntfs.mftMir - ntfs.dbr) / 2; // 备份一般位于DBR往下两簇, MFT备份 - DBR位置 / 2 = 簇
		cout << "每簇扇区数为：" << ntfs.Cu << endl;
		cout << "请输入MFT起始簇：";
		cin >> ntfs.mftStartCu;
		ntfs.mft = ntfs.mftStartCu * ntfs.Cu + ntfs.dbr; // MFT起始簇 * 簇 + DBR位置 = MFT所在扇区
		cout << "MTF所在扇区为：" << ntfs.mft << endl;

		cout << "是否再次推算簇扇区数 (Y/N)：";
		cin >> ch;
		if (ch == 'y' || ch == 'Y') {
			cout << "请输入MFT所占字节数：";
			cin >> ntfs.temp;
			cout << "请输入MFT所占簇数：";
			cin >> ntfs.mftCu;
			ntfs.Cu = (uint16_t)(ntfs.temp / 512 / ntfs.mftCu); // MFT所占字节数 / 512 / MFT所占簇数 = 簇
			cout << "(二次确认)每簇扇区数为：" << ntfs.Cu << endl;
		}
	}
	else {
		cout << "请输入MFT所在扇区 (46 49 4C 45)：";
		cin >> ntfs.mft;
		cout << "请输入MFT所占字节数：";
		cin >> ntfs.temp;
		cout << "请输入MFT所占簇数：";
		cin >> ntfs.mftCu;
		ntfs.Cu = (uint16_t)(ntfs.temp / 512 / ntfs.mftCu); // MFT所占字节数 / 512 / MFT所占簇数 = 簇
		cout << "每簇扇区数为：" << ntfs.Cu << endl;
	}

	cout << "请输入bitmap所占字节数 (一般位于MFT向下12扇区)：";
	cin >> ntfs.bitmapByte;
	ntfs.Capacity = (uint32_t)(ntfs.bitmapByte * 8 * ntfs.Cu); // 位图所占字节数 * 8 * 簇 = 分区总容量
	cout << "分区总扇区数为 (总容量)：" << ntfs.Capacity << endl;
	cout << "------------------------------------------------------" << endl;
}
// NTFS 实现完毕

// exFAT 实现
class exFAT {
public:
	uint64_t dbr, fat, bitmap, upcase, root;
	uint32_t temp, bmbyte, Cu, Capacity, sumCu, sumFat, rootCu;
}; // 定义exFAT所需变量

void Template_exFAT() {
	cout <<
		"-----------------exFAT DBR 模板------------------" << endl <<
		"                     固定值                    " << endl <<
		"EB 76 90 45 58 46 41 54 20 20 20 00 00 00 00 00" << endl <<
		"00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00" << endl <<
		"00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00" << endl <<
		"00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00" << endl <<
		"  DBR位置   --------------分区总扇区数-----------" << endl <<
		"00 80 00 00 00 00 00 00 00 70 3F 01 00 00 00 00" << endl <<
		"FAT表位置    FAT容量   DBR到位图距离 分区总簇数" << endl <<
		"00 08 00 00 40 95 00 00 00 A0 00 00 C0 83 4A 00" << endl <<
		"根目录簇数     随机值        固定值    簇 固定值" << endl <<
		"17 00 00 00 93 A2 A8 B6 00 01 00 00 09 04 01 80" << endl <<
		"-------------------------------------------------" << endl;
}

void Traverse_exFAT(exFAT& exfat) {
	cout << "请输入DBR所在扇区 (EB 76 90)：";
	cin >> exfat.dbr;
	cout << "请输入FAT表所在扇区 (F8 FF FF FF)：";
	cin >> exfat.fat;
	cout << "请确定bitamp所占簇数：";
	cin >> exfat.temp;
	cout << "保留扇区数为：" << exfat.fat - exfat.dbr << endl; // fat - dbr = 保留扇区
	cout << "请输入Bitmap所在扇区 (FF FF FF FF)：";
	cin >> exfat.bitmap;
	cout << "请输入大写字符所在扇区 (00 00 01 00)：";
	cin >> exfat.upcase;
	exfat.Cu = (exfat.upcase - exfat.bitmap) / exfat.temp; // (大小写 - 位图） / 位图所占簇数 = 每簇扇区数
	cout << "每簇扇区数为：" << exfat.Cu << endl;
	exfat.root = exfat.upcase + exfat.Cu; // 大小写 + 每簇扇区数 = 根目录位置
	cout << "根目录所在扇区为：" << exfat.root << endl;
	if (exfat.root - exfat.upcase < 16)
		exfat.rootCu = (exfat.temp + 2 + 2); // (位图所占簇数 + 起始簇号（2） + 位图所占簇数（2）) = 根目录起始簇
	else
		exfat.rootCu = (exfat.temp + 2 + 1); // (位图所占簇数 + 起始簇号（2） + 位图所占簇数（1）) = 根目录起始簇
	cout << "根目录所在簇号为：" << exfat.rootCu << endl;
	cout << "请确定bitmap所占字节数：";
	cin >> exfat.bmbyte;
	exfat.Capacity = (exfat.bmbyte * 8 * exfat.Cu) + (exfat.bitmap - exfat.dbr); // (位图字节数 * 8 * 簇) + (位图 - dbr) = 总容量
	cout << "分区总扇区数 (总容量)：" << exfat.Capacity << endl;
	exfat.sumCu = (exfat.Capacity - (exfat.bitmap - exfat.dbr)) / exfat.Cu; // (总容量 - (位图 - dbr)) / 簇 = 总簇
	cout << "总簇为：" << exfat.sumCu << endl;
	exfat.sumFat = ((uint32_t)((exfat.sumCu + 2) / 128 / exfat.Cu) + 1) * exfat.Cu; // ((总簇 + 2) / 128 / 簇 + 1) * 簇 = FAT表容量
	cout << "FAT表总扇区数(总容量)为：" << exfat.sumFat << endl;
	cout << "------------------------------------------------------" << endl;
}
// exFAT 实现完毕

// EXT 实现
class EXT {
public:
	uint64_t Superblock, Capacity, temp;
	uint32_t BlockGroup, TotalBlocks, NodeTable, iNodes, AlliNodes;
	uint16_t num, Cu;

};

void Template_EXT() {
	cout <<
		"-----------------EXT 超级块 模板-----------------" << endl <<
		"  i节点总数     块总数  -----------------------" << endl <<
		"00 05 FA 00 40 00 FA 00 03 80 0C 00 12 90 F5 00" << endl <<
		"----------------------- 簇 --------------------" << endl <<
		"F5 04 FA 00 00 00 00 00 04 00 00 00 04 00 00 00" << endl <<
		"  块组块数  -----------块i节点数-----------------" << endl <<
		"F8 FF 00 00 F8 FF 00 00 00 FF 00 00 00 00 00 00" << endl <<
		"------------------    固定值    ---------------" << endl <<
		"00 00 00 00 00 00 FF FF 53 EF 01 00 01 00 00 00" << endl <<
		"00 00 00 00 00 00 00 00 00 00 00 00 01 00 00 00" << endl <<
		"-----------------------------编号--------------" << endl <<
		"00 00 00 00 0B 00 00 00 00 01 00 00 3C 00 00 00" << endl <<
		"-------------------------------------------------" << endl;
}

void Traverse_EXT(EXT& ext) {
	cout << "--------------------------------------------------------------------------------------------------" << endl;
	char ch;
	int temp = 2;
	cout << "向下搜索 53 EF 偏移为 512 = 56, 如果搜索后为备份, 建议多搜几次以保证前几个备份没有被修改" << endl;
	cout << "请输入当前超级块或备份所在扇区（填入含有53 EF的扇区, 并非空白扇区）：";
	cin >> ext.Superblock; // Superblock = 超级块
	cout << "当前超级块编号是否为0号 (Y/N)：";
	cin >> ch;
	if (ch != 'Y' && ch != 'y') {
		cout << "请输入每块组块数：";
		cin >> ext.BlockGroup; // BlockGroup = 块组
		cout << "请输入当前超级块编号：";
		cin >> ext.num;
		cout << "请输入簇块数：";
		cin >> ext.Cu;
		ext.Superblock = ext.Superblock - (uint64_t)(ext.BlockGroup) * ext.num * ext.Cu; // 块组 * 编号 * 簇 = 0号偏移到当前编号扇区数
		cout << "0号超级块所在扇区号为：" << ext.Superblock << endl;
		cout << "复制备份后, 在0号超级块向下两扇区粘贴。之后的跳转位置均从空白扇区跳转" << endl;
		cout << "请输入总块数：";
		cin >> ext.TotalBlocks;
		ext.Capacity = (uint64_t)ext.TotalBlocks * ext.Cu; // 总块数 * 簇 = 分区总扇区数
		cout << "分区总扇区数（总容量）为：" << ext.Capacity << endl;
		cout << "请输入每块组包含的 i节点 数：";
		cin >> ext.iNodes; // iNodes = i节点数 
	}
	else {
		ext.Superblock -= 2;
		cout << "请输入总块数：";
		cin >> ext.TotalBlocks;
		cout << "请输入簇块数：";
		cin >> ext.Cu;
		ext.Capacity = (uint64_t)ext.TotalBlocks * ext.Cu; // 总块数 * 簇 = 分区总扇区数
		cout << "分区总扇区数（总容量）为：" << ext.Capacity << endl;
		cout << "之后的跳转位置均从空白扇区跳转" << endl;
		cout << "请输入每块组块数：";
		cin >> ext.BlockGroup; // BlockGroup = 块组
		cout << "请输入每块组包含的 i节点 数：";
		cin >> ext.iNodes; // iNodes = i节点数 
	}

	cout << "向下跳转" << ext.Cu << "到达节点描述符" << endl;
	cout << "请输入第一个 节点表 起始簇：";
	cin >> ext.NodeTable; // NodeTable = 节点表起始簇
	cout << "跳转" << ext.NodeTable * ext.Cu << "扇区到达第1个节点表" << endl;
	cout << "是否遍历节点表 (Y/N)：";
	cin >> ch;
	if (ch == 'Y' || ch == 'y') {
		ext.AlliNodes = ext.TotalBlocks / ext.BlockGroup + 1; // 总块数 / 块组数 + 1 = i节点表总数
		cout << "是否遍历所有节点表 (Y/N)：";
		cin >> ch;
		if (ch == 'y' || ch == 'Y') {
			ext.temp = (uint64_t)ext.NodeTable + ext.BlockGroup; // （节点表起始簇 + 块组数）* 32 = 第二个节点表所在扇区
			cout << "第2个节点表所在扇区为：" << ext.temp * 32 << endl;
			for (uint32_t i = 2; i != ext.AlliNodes; ++i)
				cout << "第" << i + 1 << "个节点表所在扇区为：" << ((uint64_t)(ext.BlockGroup) * i + 2) * (uint64_t)ext.Cu << endl;
			// 块组 * i = i编号的位图节点位置，+2 = 偏移两簇， * 簇 = 节点表位置。
		}
		else {
			cout << "节点表总数为：" << ext.AlliNodes << endl;
			cout << "请输入遍历个数 (不要大于节点表总数)：";
			cin >> temp;
			if (temp <= ext.AlliNodes) {
				ext.temp = (uint64_t)ext.NodeTable + ext.BlockGroup; // （节点表起始簇 + 块组数）* 32 = 第二个节点表所在扇区
				cout << "第2个节点表所在扇区为：" << ext.temp * 32 << endl;
				for (uint32_t i = 2; i != temp; ++i)
					cout << "第" << i + 1 << "个节点表所在扇区为：" << ((uint64_t)(ext.BlockGroup) * i + 2) * (uint64_t)ext.Cu << endl;
				// 块组 * i = i编号的位图节点位置，+2 = 偏移两簇， * 簇 = 节点表位置。
			}
			else
				cerr << "输入错误，停止遍历" << endl;
		}
	}
	cout << "--------------------------------------------------------------------------------------------------" << endl;
} // 遍历EXT
// EXT 实现完毕

// 设置密文与明文
void setCipher(vector<uint16_t>& Cipher) {
	int temp;
	char ch;
	cout << "请保证密文和明文字节数量对应" << endl;
	cout << "请输入密文 (输入十六进制单字节, 空格隔开)：";
	while ((cin >> hex >> temp).get(ch)) {
		Cipher.push_back(temp);
		if (ch == '\n') break;
	}
} // 读取密文
bool setPlaintext(vector<uint16_t>& Plaintext, vector<uint16_t> Cipher) {
	int temp;
	char ch;
	cout << "请输入明文 (输入十六进制单字节, 空格隔开)：";
	while ((cin >> hex >> temp).get(ch)) {
		Plaintext.push_back(temp);
		if (ch == '\n') break;
	}
	if (Cipher.size() != Plaintext.size()) {
		cout << endl << "密文与明文数量不对应" << endl << endl;
		system("pause");
		return false;
	}
	return true;
} // 读取明文

// 异或减与加减异或
void xor_and_dec(vector<uint16_t> Plaintext, vector<uint16_t> Cipher) {
	vector<uint16_t> resXor, result, copyPlain;
	int temp, j = 0;
	for (int n = 1; n != 256; ++n) {
		copyPlain = Plaintext;

		for (auto s : Cipher)
			resXor.push_back(s ^ n); // a1 b1 c1 d1

		temp = ((resXor.back() + 256) - Plaintext.back()) % 256; // d1 + 256 - m4 % 256

		for (auto s : resXor)
			result.push_back(((s + 256) - temp) % 256); // a1 + 256 - d1 % 256
		result.pop_back();
		copyPlain.pop_back(); // ce cf 0d
		if (result == copyPlain) {
			j++;
			if (j == 1) cout << endl << "----------异或加减----------" << endl << endl;
			cout << "先异或：" << setfill('0') << setw(2) << hex << uppercase << n << " 再减去：" << temp << endl;
		}
		result.clear();
		resXor.clear();
		if (n == 255 && j != 0) cout << endl << "--------异或加减结束--------" << endl;
	}
} // 进行异或加减(xor and dec)
void add_and_xor(vector<uint16_t> Plaintext, vector<uint16_t> Cipher) {
	vector<uint16_t> resAdd, result, copyPlain;
	int temp, j = 0;
	for (int n = 1; n != 256; ++n) {
		copyPlain = Plaintext;
		for (auto s : Cipher)
			resAdd.push_back((s + n) % 256); // (a + n) % 256

		temp = (resAdd.back() ^ Plaintext.back()); // (d1 ^ m4)

		for (auto s : resAdd)
			result.push_back(s ^ temp); // (a1 ^ d1)
		result.pop_back(); // remove (d1 ^ d1)
		copyPlain.pop_back(); // remove (m4)

		if (result == copyPlain) {
			j++;
			if (j == 1) cout << endl << "----------加减异或----------" << endl << endl;
			cout << "先加：" << setfill('0') << setw(2) << hex << uppercase << n << " 再异或：" << temp << endl;

		}
		result.clear();
		resAdd.clear();
		if (n == 255 && j != 0) cout << endl << "--------加减异或结束--------" << endl;
	}
}

// 左旋
void Levo(vector<uint16_t> Plaintext, vector<uint16_t> Cipher) {
	int n = 0;
	vector<uint16_t> vtimes;
	cout << endl << "----------左旋开始----------" << endl << endl;
	for (const auto s : Cipher) {
		int result, Cp = s;
		for (int times = 1; times != 9; ++times) {
			result = (uint16_t)(((Cp * pow(2, times)) / 256) + ((Cp * (uint16_t)pow(2, times)) % 256));
			if (result == Plaintext[n]) {
				cout << setfill('0') << setw(2) << hex << uppercase << Cp << " 左旋次数为：" << times << "次" << endl;
				n++;
				vtimes.push_back(times);
				break;
			}
			else if (result != Plaintext[n] && times == 8) {
				cout << setfill('0') << setw(2) << hex << uppercase << Cp << " 不属于左旋解密" << endl;
				n++;
				vtimes.push_back(255);
				break;
			}
		}
	}
	cout << endl << "----------左旋结束----------" << endl;
}

// 异或
void Xor(vector<uint16_t> Plaintext, vector<uint16_t> Cipher) {
	cout << endl << "----------异或开始----------" << endl << endl;
	int temp = 0;
	cout << "密文 Xor 明文 = 结果" << endl << endl;
	cout << "密文：";
	for (auto s : Cipher) cout << s << " "; cout << endl;
	cout << "明文：";
	for (auto s : Plaintext) cout << s << " "; cout << endl;

	cout << "结果：";
	for (auto i = 0; i != Cipher.size(); ++i) {
		temp = Cipher[i] ^ Plaintext[i];
		cout << setfill('0') << setw(2) << hex << uppercase << temp << " ";
	} cout << endl;
	cout << endl << "----------异或结束----------" << endl;
}

// 读取文件 左侧索引函数
void LeftIndex(int num, char* hexNumber) {
	for (int i = 0; i < 8; i++) {
		hexNumber[i] = '0';
	}
	int index = 7;
	while (num != 0 && index >= 0) {
		hexNumber[index--] = HEX[num % 16];
		num = num / 16;
	}
}

// 读取文件 主体函数
vector<uint16_t> ReadFile(string& filename) {
	vector<uint16_t> cfile;
	uint8_t temp;
	char ch;

	cout << "拖入文件：";
	cin >> filename;
	ifstream infile(filename, ios::binary);

	cout << "是否输出文件内容 (Y/N)：";
	cin >> ch;

	if (ch == 'y' || ch == 'Y') {
		// 左边第一行索引
		int num = 0;
		char hexNumber[9] = "00000000";
		cout << "\t     ";
		for (int i = 0; i < 16; i++) cout << HEX[i] << "  ";
		cout << endl;
		while (!infile.eof()) {
			for (int i = 0; i != 16; ++i) {
				if (infile.eof()) break;
				if (num % 16 == 0) { // 第一行之后的索引
					LeftIndex(num, hexNumber);
					cout << hexNumber << ":    ";
				}
				num++;
				//十六进制数据显示
				infile.read((char*)&temp, 1);
				cfile.push_back(temp);
				cout << setfill('0') << setw(2) << hex << uppercase << cfile.back() << " ";
			} cout << endl;
		}
		cfile.pop_back();
		cout << endl << "请不要在意为什么多打印了一个，我修复不了，但是仅仅只是打印出了问题，能运行就行了，不信你看他所占字节数：" << dec << cfile.size() << endl;
	}
	else {
		while (!infile.eof()) {
			infile.read((char*)&temp, 1);
			cfile.push_back(temp);
		}
		cfile.pop_back();
		cout << "------------------------------" << endl;
		cout << "文件所占字节数为：" << dec << cfile.size() << endl;
	}

	filename = filename.substr(filename.size() - 4, filename.back()); // 返回文件类型 如：.jpg
	return cfile;
}

// 文件的简单解密
void jpgDecryption(vector<uint16_t> jpg) { // Decryption = 解密
	vector<uint16_t> jpgCipher = { jpg[0], jpg[1], jpg[2], jpg[3], jpg[jpg.size() - 1], jpg[jpg.size() - 2] };
	cout << "jpg密文为：";
	for (auto s : jpgCipher) cout << setfill('0') << setw(2) << hex << uppercase << s << " ";
	cout << endl;
	cout << "jpg明文为：";
	for (auto s : jpgPlaintext) cout << setfill('0') << setw(2) << hex << uppercase << s << " ";
	cout << endl << "明文与密文均为一一对应关系" << endl;
	cout << "------------------------------" << endl;
	if (jpgPlaintext == jpgCipher) cout << "明文与密文相等, 不属于 异或 和 左旋" << endl;
	else {
		xor_and_dec(jpgPlaintext, jpgCipher); // jpgPlaintext{ 0xFF, 0xD8, 0xFF, 0xE0, 0xD9, 0xFF };
		add_and_xor(jpgPlaintext, jpgCipher);
		cout << endl;
		Levo(jpgPlaintext, jpgCipher);
	}
}
void pngDecryption(vector<uint16_t> png) {
	vector<uint16_t> pngCipher{ png[0], png[1], png[2], png[3], png[4], png[5], png[6], png[7] };
	cout << "密文为：";
	for (auto s : pngCipher) cout << setfill('0') << setw(2) << hex << uppercase << s << " ";
	cout << endl;
	cout << "明文为：";
	for (auto s : pngPlaintext) cout << setfill('0') << setw(2) << hex << uppercase << s << " ";
	cout << endl;
	if (pngCipher == pngPlaintext) cout << "明文与密文相等, 不属于 异或 和 左旋" << endl;
	else {
		xor_and_dec(pngPlaintext, pngCipher);
		add_and_xor(pngPlaintext, pngCipher);
		cout << endl;
		Levo(pngPlaintext, pngCipher);
	}
}
void docDecryption(vector<uint16_t> doc) {
	vector<uint16_t> docCipher{ doc[0], doc[1], doc[2], doc[3], doc[4], doc[5], doc[6], doc[7] };
	cout << "密文为：";
	for (auto s : docCipher) cout << setfill('0') << setw(2) << hex << uppercase << s << " ";
	cout << endl;
	cout << "明文为：";
	for (auto s : docPlaintext) cout << setfill('0') << setw(2) << hex << uppercase << s << " ";
	cout << endl;
	if (docCipher == docPlaintext) cout << "明文与密文相等, 不属于 异或 和 左旋" << endl;
	else {
		xor_and_dec(docPlaintext, docCipher); // docPlaintext{ 0xD0, 0xCF, 0x11, 0xE0, 0xA1, 0xB1, 0x1A, 0xE1 };
		add_and_xor(docPlaintext, docCipher);
		cout << endl;
		Levo(docPlaintext, docCipher);
	}
}
void docxDecryption(vector<uint16_t> docx) {
	vector<uint16_t> docxCipher{ docx[0], docx[1], docx[2], docx[3] };
	cout << "密文为：";
	for (auto s : docxCipher) cout << setfill('0') << setw(2) << hex << uppercase << s << " ";
	cout << endl;
	cout << "明文为：";
	for (auto s : docxPlaintext) cout << setfill('0') << setw(2) << hex << uppercase << s << " ";
	cout << endl;
	if (docxCipher == docxPlaintext) cout << "明文与密文相等, 不属于 异或 和 左旋" << endl;
	else {
		xor_and_dec(docxPlaintext, docxCipher);
		add_and_xor(docxPlaintext, docxCipher);
		cout << endl;
		Levo(docxPlaintext, docxCipher);
	}
}
void bmpDecryption(vector<uint16_t> bmp) {
	vector<uint16_t> bmpCipher{ bmp[0], bmp[1], bmp[10], bmp[14], bmp[15] };
	cout << "密文为：";
	for (auto s : bmpCipher) cout << setfill('0') << setw(2) << hex << uppercase << s << " ";
	cout << endl;
	cout << "明文为：";
	for (auto s : bmpPlaintext) cout << setfill('0') << setw(2) << hex << uppercase << s << " ";
	cout << endl;
	if (bmpCipher == bmpPlaintext) cout << "明文与密文相等, 不属于 异或 和 左旋" << endl;
	else {
		xor_and_dec(bmpPlaintext, bmpCipher);
		add_and_xor(bmpPlaintext, bmpCipher);
		cout << endl;
		Levo(bmpPlaintext, bmpCipher);
	}
}
void rtfDecryption(vector<uint16_t> rtf) {
	vector<uint16_t> rtfCipher{ rtf[0], rtf[1], rtf[2], rtf[3], rtf[4] };
	cout << "密文为：";
	for (auto s : rtfCipher) cout << setfill('0') << setw(2) << hex << uppercase << s << " ";
	cout << endl;
	cout << "明文为：";
	for (auto s : rtfPlaintext) cout << setfill('0') << setw(2) << hex << uppercase << s << " ";
	cout << endl;
	if (rtfCipher == rtfPlaintext)	cout << "明文与密文相等, 不属于异或 和 左旋" << endl;
	else {
		xor_and_dec(rtfPlaintext, rtfCipher);
		add_and_xor(rtfPlaintext, rtfCipher);
		cout << endl;
		Levo(rtfPlaintext, rtfCipher);
	}
}
void gifDecryption(vector<uint16_t> gif) {
	vector<uint16_t> gifCipher{ gif[0], gif[1], gif[2], gif[3], gif[4], gif[5] };
	cout << "密文为：";
	for (auto s : gifCipher) cout << setfill('0') << setw(2) << hex << uppercase << s << " ";
	cout << endl;
	cout << "明文为：";
	for (auto s : gifPlaintext) cout << setfill('0') << setw(2) << hex << uppercase << s << " ";
	cout << endl;
	if (gifCipher == gifPlaintext) cout << "明文与密文相等, 不属于异或 和 左旋" << endl;
	else {
		xor_and_dec(gifPlaintext, gifCipher);
		add_and_xor(gifPlaintext, gifCipher);
		cout << endl;
		Levo(gifPlaintext, gifCipher);
	}
}

// 判断文件类型，并调用该文件的解密函数
void Choicefile(const string filename, vector<uint16_t> fileContent) {
	if (filename == ".jpg")
		jpgDecryption(fileContent);
	if (filename == ".png")
		pngDecryption(fileContent);
	if (filename == ".doc" || filename == ".xls" || filename == "ppt")
		docDecryption(fileContent);
	if (filename == "docx" || filename == "pptx" || filename == "xlsx")
		docxDecryption(fileContent);
	if (filename == ".bmp")
		bmpDecryption(fileContent);
	if (filename == ".rtf")
		rtfDecryption(fileContent);
	if (filename == ".gif")
		gifDecryption(fileContent);
}

// jpg 文件结构修复
class JPG {
public:
	int E0 = 0, C0 = 0, DA = 0, DAend = 0;
	vector<int> DB{ 0, 0 }, C4{ 0, 0, 0, 0 };
};

void jpgStructure() {
	cout << "------------------------------------------------------------" << endl;
	cout << "只支持windows自带的画图软件制作的jpg文件" << endl;
	vector<uint16_t> jpgfile, jpgCopy;
	string filename;
	jpgfile = ReadFile(filename);
	jpgCopy = jpgfile;
	JPG* jpg = new JPG(); // int E0, DB, C0, C4, DA;

	int index = 1;
	for (auto s = jpgfile.begin(); s != jpgfile.end(); ++s, ++index) {
		if (*s == 0xFF) {
			switch (*(s + 1)) {
			case 0xE0:
				jpg->E0 = index;
				break;
			case 0xDB:
				if (jpg->DB[0] == 0)
					jpg->DB[0] = index;
				else if (jpg->DB[1] == 0)
					jpg->DB[1] = index;
				break;
			case 0xC0:
				jpg->C0 = index;
				break;
			case 0xC4:
				if (jpg->C4[0] == 0)
					jpg->C4[0] = index;
				else if (jpg->C4[1] == 0)
					jpg->C4[1] = index;
				else if (jpg->C4[2] == 0)
					jpg->C4[2] = index;
				else if (jpg->C4[3] == 0)
					jpg->C4[3] = index;
				break;
			case 0xDA:
				jpg->DA = index;
				break;
			}
		}
		if (*(s + 1) == 0x3F) {
			jpg->DAend = index + 1;
			break;
		}
	}

	cout << "------------------------------------------------------------" << endl;
	uint16_t temp;
	temp = (jpgfile[jpg->E0 + 1] * 256) + jpgfile[jpg->E0 + 2];
	//cout << "FF E0偏移量为：" << setfill('0') << setw(2) << hex << temp << endl;
	if (jpgfile[jpg->E0 + 1 + temp + 1] != 0xDB) {
		cout << "FF E0偏移量错误, 应为：" << setfill('0') << setw(2) << hex << uppercase << jpg->DB[0] - jpg->E0 - 2 << endl;
		jpgCopy[jpg->E0 + 1] = (jpg->DB[0] - jpg->E0 - 2) / 256;
		jpgCopy[jpg->E0 + 2] = (jpg->DB[0] - jpg->E0 - 2) % 256;
	}

	temp = (jpgfile[jpg->DB[0] + 1] * 256) + jpgfile[jpg->DB[0] + 2];
	//cout << "第一个FF DB偏移量为：" << setfill('0') << setw(2) << hex << uppercase << temp << '\t';
	if (jpgfile[jpg->DB[0] + 1 + temp + 1] != 0xDB) {
		cout << "第一个FF DB偏移量错误, 应为：" << setfill('0') << setw(2) << hex << uppercase << jpg->DB[1] - jpg->DB[0] - 2 << '\t';
		jpgCopy[jpg->DB[0] + 1] = (jpg->DB[1] - jpg->DB[0] - 2) / 256;
		jpgCopy[jpg->DB[0] + 2] = (jpg->DB[1] - jpg->DB[0] - 2) % 256;
	}
	if (jpgfile[jpg->DB[0] + 3] != 0x00) {
		cout << "量化表为：" << setfill('0') << setw(2) << hex << uppercase << jpgfile[jpg->DB[0] + 3] << " 错误" << endl;
		jpgCopy[jpg->DB[0] + 3] = 0x00;
	}

	temp = (jpgfile[jpg->DB[1] + 1] * 256) + jpgfile[jpg->DB[1] + 2];
	//cout << "第二个FF DB偏移量为：" << setfill('0') << setw(2) << hex << temp << '\t';
	if (jpgfile[jpg->DB[1] + 1 + temp + 1] != 0xC0) {
		cout << "第二个FF DB偏移量错误, 应为：" << setfill('0') << setw(2) << hex << uppercase << jpg->C0 - jpg->DB[1] - 2 << '\t';
		jpgCopy[jpg->DB[1] + 1] = (jpg->C0 - jpg->DB[1] - 2) / 256;
		jpgCopy[jpg->DB[1] + 2] = (jpg->C0 - jpg->DB[1] - 2) % 256;
	}
	if (jpgfile[jpg->DB[1] + 3] != 0x01) {
		cout << "量化表为：" << setfill('0') << setw(2) << hex << uppercase << jpgfile[jpg->DB[1] + 3] << " 错误" << endl;
		jpgCopy[jpg->DB[1] + 3] = 0x01;
	}

	temp = (jpgfile[jpg->C0 + 1] * 256) + jpgfile[jpg->C0 + 2];
	//cout << "FF C0偏移量为：" << setfill('0') << setw(2) << hex << temp << '\t';
	if (jpgfile[jpg->C0 + 1 + temp + 1] != 0xC4) {
		cout << "FF C0偏移量错误, 应为：" << setfill('0') << setw(2) << hex << uppercase << jpg->C4[0] - jpg->C0 << '\t';
		jpgCopy[jpg->C0 + 1] = (jpg->C4[0] - jpg->C0 - 2) / 256;
		jpgCopy[jpg->C0 + 2] = (jpg->C4[0] - jpg->C0 - 2) % 256;
	}
	if (jpgfile[jpg->C0 + 3] != 0x08) {
		cout << "样本精度错误, 应为：08" << endl;
		jpgCopy[jpg->C0 + 3] = 0x08;
	}
	if (jpgfile[jpg->C0 + 8] != 0x03) {
		cout << "组件数量错误, 当前值为：" << setfill('0') << setw(2) << hex << uppercase << jpgfile[jpg->C0 + 8] << " 应为：03" << endl;
		jpgCopy[jpg->C0 + 8] = 0x03;
	}
	if (jpgfile[jpg->C0 + 9] != 0x01 || jpgfile[jpg->C0 + 10] != 0x22 || jpgfile[jpg->C0 + 11] != 0x00) {
		cout << "垂直采样率错误, 当前值为：" << setfill('0') << setw(2) << hex << uppercase <<
			jpgfile[jpg->C0 + 9] << " " << jpgfile[jpg->C0 + 10] << " " << setfill('0') << setw(2) << uppercase << jpgfile[jpg->C0 + 11] <<
			" 应为：01 22 00" << endl;
		jpgCopy[jpg->C0 + 9] = 0x01, jpgCopy[jpg->C0 + 10] = 0x22, jpgCopy[jpg->C0 + 11] = 0x00;
	}
	if (jpgfile[jpg->C0 + 12] != 0x02 || jpgfile[jpg->C0 + 13] != 0x11 || jpgfile[jpg->C0 + 14] != 0x01) {
		cout << "水平采样率错误, 当前值为：" << setfill('0') << setw(2) << hex << uppercase <<
			jpgfile[jpg->C0 + 12] << " " << jpgfile[jpg->C0 + 13] << " " << setfill('0') << setw(2) << uppercase << jpgfile[jpg->C0 + 14] <<
			" 应为：02 11 01" << endl;
		jpgCopy[jpg->C0 + 12] = 0x02, jpgCopy[jpg->C0 + 13] = 0x11, jpgCopy[jpg->C0 + 14] = 0x01;
	}
	if (jpgfile[jpg->C0 + 15] != 0x03 || jpgfile[jpg->C0 + 16] != 0x11 || jpgfile[jpg->C0 + 17] != 0x01) {
		cout << "水平采样率错误, 当前值为：" << setfill('0') << setw(2) << hex << uppercase <<
			jpgfile[jpg->C0 + 15] << " " << jpgfile[jpg->C0 + 16] << " " << setfill('0') << setw(2) << uppercase << jpgfile[jpg->C0 + 17] <<
			"应为：03 11 01" << endl;
		jpgCopy[jpg->C0 + 15] = 0x03, jpgCopy[jpg->C0 + 16] = 0x11, jpgCopy[jpg->C0 + 17] = 0x01;
	}

	for (int i = 0; i != 4; i++) {
		temp = (jpgfile[jpg->C4[i] + 1] * 256) + jpgfile[jpg->C4[i] + 2];
		//cout << "第 " << i + 1 <<" 个FF C4偏移量为：" << setfill('0') << setw(2) << hex << uppercase << temp << '\t';
		if (i != 3) {
			if (jpgfile[jpg->C4[i] + 1 + temp + 1] != 0xC4) {
				cout << "第 " << i + 1 << " 个FF C4偏移量错误, 应为：" << setfill('0') << setw(2) << hex << uppercase << jpg->C4[i + 1] - jpg->C4[i] - 2 << endl;
				jpgCopy[jpg->C4[i] + 1] = (jpg->C4[i + 1] - jpg->C4[i] - 2) / 256;
				jpgCopy[jpg->C4[i] + 2] = (jpg->C4[i + 1] - jpg->C4[i] - 2) % 256;
			}
		}
		else if (i == 3) {
			temp = (jpgfile[jpg->C4[3] + 1] * 256) + jpgfile[jpg->C4[3] + 2];
			if (jpgfile[jpg->C4[3] + 1 + temp + 1] != 0xDA) {
				cout << "第 " << 4 << " 个FF C4偏移量错误, 应为：" << setfill('0') << setw(2) << hex << uppercase << jpg->DA - jpg->C4[3] - 2 << endl;
				jpgCopy[jpg->C4[3] + 1] = (jpg->DA - jpg->C4[3] - 2) / 256;
				jpgCopy[jpg->C4[3] + 2] = (jpg->DA - jpg->C4[3] - 2) % 256;
			}
		}
		switch (i) {
		case 0:
			if (jpgfile[jpg->C4[i] + 3] != 0x00) {
				cout << "霍夫曼表错误, 应为 00" << endl;
				jpgCopy[jpg->C4[i] + 3] = 0x00;
			}break;
		case 1:
			if (jpgfile[jpg->C4[i] + 3] != 0x10) {
				cout << "霍夫曼表错误, 应为 10" << endl;
				jpgCopy[jpg->C4[i] + 3] = 0x10;
			}break;
		case 2:
			if (jpgfile[jpg->C4[i] + 3] != 0x01) {
				cout << "霍夫曼表错误, 应为 01" << endl;
				jpgCopy[jpg->C4[i] + 3] = 0x01;
			}break;
		case 3:
			if (jpgfile[jpg->C4[3] + 3] != 0x11) {
				cout << "霍夫曼表错误, 应为 01" << endl;
				jpgCopy[jpg->C4[3] + 3] = 0x11;
			}break;
		}
	}

	temp = (jpgfile[jpg->DA + 1] * 256) + jpgfile[jpg->DA + 2];
	//cout << "FF DA偏移量为：" << setfill('0') << setw(2) << hex << uppercase << temp << '\t';
	if (jpg->DA + 1 + temp != 0x3F) {
		cout << "FF DA偏移量错误, 应为：" << setfill('0') << setw(2) << hex << uppercase << jpg->DAend - jpg->DA << endl;
		jpgCopy[jpg->DA + 1] = (jpg->DAend - jpg->DA) / 256;
		jpgCopy[jpg->DA + 2] = (jpg->DAend - jpg->DA) % 256;
	}
	if (jpgfile[jpg->DA + 3] != 0x03) {
		cout << "组件数量错误, 应为：03" << endl;
		jpgCopy[jpg->DA + 3] = 0x03;
	}
	if (jpgfile[jpg->DA + 4] != 0x01 || jpgfile[jpg->DA + 5] != 0x00) {
		cout << "1号组件错误, 应为：01 00" << endl;
		jpgCopy[jpg->DA + 4] = 0x01;
		jpgCopy[jpg->DA + 5] = 0x00;
	}
	if (jpgfile[jpg->DA + 6] != 0x02 || jpgfile[jpg->DA + 7] != 0x11) {
		cout << "2号组件错误, 应为：02 11" << endl;
		jpgCopy[jpg->DA + 6] = 0x02;
		jpgCopy[jpg->DA + 7] = 0x11;
	}
	if (jpgfile[jpg->DA + 8] != 0x03 || jpgfile[jpg->DA + 9] != 0x11) {
		cout << "3号组件错误, 应为：03 11" << endl;
		jpgCopy[jpg->DA + 8] = 0x03;
		jpgCopy[jpg->DA + 9] = 0x11;
	}
	cout << "------------------------------------------------------------" << endl;

	// 输出文件
	ofstream outfile;
	auto j = 0;
	for (auto i = 0; i != jpgCopy.size(); i += 10000) {
		outfile.open("D:\\demo.jpg", ios::app | ios::binary);
		for (j = i; j != i + 10000; j++) {
			outfile << (char)jpgCopy[j];
			if (j == jpgCopy.size() - 1) break;
		}
		outfile.close();
		if (j == jpgCopy.size() - 1) break;
	}
	cout << "生成文件所在位置为：D:\\demo.jpg" << endl;
	cout << "------------------------------------------------------------" << endl;
}

// doc 文件结构修复
class DOC {
public:
	const vector<uint16_t> docFix{
		0xD0,0xCF,0x11,0xE0,0xA1,0xB1,0x1A,0xE1,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3E,0x00,0x03,0x00,0xFE,0xFF,0x09,0x00,
		0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };
	uint32_t SAT = 0, SATnum = 0, root = 0, SSAT = 0, SSATnum = 0, MSATnum = 0, MSAT = 0, FstSAT = 0, ECA5 = 0;
	vector<uint32_t> SID;
};

void docStructure() {
	DOC* doc = new DOC();
	string filename;
	vector<uint16_t> docfile = ReadFile(filename);
	vector<uint16_t> docCopy = docfile;

	for (auto i = 0; i != 44; ++i) {
		docCopy[i] = doc->docFix[i];
		// cout << setfill('0') << setw(2) << hex << uppercase << docCopy[i] << " ";
	} // cout << endl;

	// 搜索 SAT 表个数 及 SID
	cout << "算法有错，只支持某些doc。" << endl;
	int SID = 0;
	for (auto i = 0; i != docfile.size(); ++i) {
		if (docfile[i] == 0xFD && docfile[i + 1] == 0xFF && docfile[i + 2] == 0xFF && docfile[i + 3] == 0xFF) {
			if (docfile[i + 4] == 0xFD && docfile[i + 5] == 0xFF && docfile[i + 6] == 0xFF && docfile[i + 7] == 0xFF) {
				doc->SATnum++;
				int j = i, jNum = 0;
				while (true) {
					if (i % 512 == 0) {
						doc->SID.push_back(j / 512 - 1);
						break;
					}
					else if (docfile[j - 1] != 0xFF && docfile[j - 2] != 0xFF && docfile[j - 3] != 0xFF && (docfile[j - 4] != 0xFF || docfile[j - 4] != 0xFE)) {
						uint32_t SATtemp;
						SATtemp = (docfile[j - 1] * 256 * 256 * 256) + (docfile[j - 2] * 256 * 256) + (docfile[j - 3] * 256) + (docfile[j - 4]);
						doc->SID.push_back(SATtemp + jNum);
						break;
					}
					j -= 4, jNum++;
				}
			}
			else if (docfile[i - 4] == 0xFD && docfile[i - 5] == 0xFF && docfile[i - 6] == 0xFF && docfile[i - 7] == 0xFF) {
				doc->SATnum++;
				int j = i, jNum = 0;
				while (true) {
					if (i % 512 == 0) {
						doc->SID.push_back(j / 512 - 1);
						break;
					}
					else if (docfile[j - 1] != 0xFF && docfile[j - 2] != 0xFF && docfile[j - 3] != 0xFF && (docfile[j - 4] != 0xFF || docfile[j - 4] != 0xFE)) {
						uint32_t SATtemp;
						SATtemp = (docfile[j - 1] * 256 * 256 * 256) + (docfile[j - 2] * 256 * 256) + (docfile[j - 3] * 256) + (docfile[j - 4]);
						doc->SID.push_back(SATtemp + jNum);
						break;
					}
					j -= 4, jNum++;
				}
			}
			else {
				doc->SATnum++;
				int j = i, jNum = 0;
				while (true) {
					if (i % 512 == 0) {
						doc->SID.push_back(j / 512 - 1);
						break;
					}
					else if (docfile[j - 1] != 0xFF && docfile[j - 2] != 0xFF && docfile[j - 3] != 0xFF && (docfile[j - 4] != 0xFF || docfile[j - 4] != 0xFE)) {
						uint32_t SATtemp;
						SATtemp = (docfile[j - 1] * 256 * 256 * 256) + (docfile[j - 2] * 256 * 256) + (docfile[j - 3] * 256) + (docfile[j - 4]);
						doc->SID.push_back(SATtemp - 1 + jNum);
						break;
					}
					j -= 4, jNum++;
				}
			}
		}
	}	cout << "SAT表个数为：" << dec << doc->SATnum << endl;

	if (doc->SATnum > 256) {
		docCopy[44] = doc->SATnum % 256, docCopy[45] = doc->SATnum % 65536 / 256;
		docCopy[46] = doc->SATnum % 16777216 / 65536, docCopy[47] = doc->SATnum % 4294967296 / 16777216;
	}
	else
		docCopy[44] = doc->SATnum, docCopy[45] == 0, docCopy[46] == 0, docCopy[47] == 0;

	int num = 76;
	for (auto s : doc->SID) {
		docCopy[num++] = s % 256, docCopy[num++] = s % 65536 / 256;
		docCopy[num++] = s % 16777216 / 65536, docCopy[num++] = s % 4294967296 / 16777216;
	}

	if (doc->SATnum == 1) {
		cout << "第一个SAT表所在扇区为：" << dec << ++doc->SID[0] + 1 << " 填入MSAT表应为：" << hex << uppercase << doc->SID[0] << endl;
		docCopy[76] = doc->SID[0] % 256, docCopy[77] = doc->SID[0] % 65536 / 256;
		docCopy[78] = doc->SID[0] % 16777216 / 65536, docCopy[79] = doc->SID[0] % 4294967296 / 16777216;
	}
	else
		for (auto s : doc->SID) cout << dec << "SID：" << s << endl;
	// root
	for (auto i = 0; i != docfile.size(); ++i)
		if (docfile[i] == 0x52 && docfile[i + 2] == 0x6F && docfile[i + 4] == 0x6F && docfile[i + 6] == 0x74)
			doc->root = i / 512 - 1;
	docCopy[48] = doc->root;

	cout << "Root：" << dec << doc->root << endl;
	// ssat
	for (auto i = 0; i != docfile.size(); ++i)
		if (docfile[i] == 0x01 && docfile[i + 1] == 0x00 && docfile[i + 2] == 0x00 && docfile[i + 3] == 0x00)
			if (i % 512 == 0) {
				for (auto s : doc->SID) {
					if (i / 512 == s + 1) break;
					else doc->SSAT = i / 512 - 1;
				}
			}
	cout << "SSAT:" << doc->SSAT << endl;
	doc->SSATnum++;
	docCopy[60] = doc->SSAT;
	docCopy[64] = doc->SSATnum;

	for (auto i = 0; i != docfile.size(); ++i)
		if (docfile[i] == 0x57 && docfile[i + 2] == 0x6F && docfile[i + 4] == 0x72 && docfile[i + 6] == 0x64 && docfile[i + 8] == 0x44) {
			doc->ECA5 = (docfile[i + 116 + 3] * 256 * 256 * 256) + (docfile[i + 2 + 116] * 256 * 256) + (docfile[i + 1 + 116] * 256) + (docfile[i + 116]);
			break;
		}
	cout << "ECA5（可能不正确）:" << dec << doc->ECA5 << endl;

	// 输出文件
	ofstream outfile;
	auto j = 0;
	for (auto i = 0; i != docCopy.size(); i += 10000) {
		outfile.open("D:\\demo.doc", ios::app | ios::binary);
		for (j = i; j != i + 10000; j++) {
			outfile << (char)docCopy[j];
			if (j == docCopy.size() - 1) break;
		}
		outfile.close();
		if (j == docCopy.size() - 1) break;
	}
	cout << "生成文件所在位置为：D:\\demo.doc" << endl;
	cout << "------------------------------------------------------------" << endl;
}

int Choose(int& index) {
	cout << "-------------------------------------------------------" << endl;
	cout << "1. FAT32    2. NTFS    3. exFAT    4. EXT" << endl;
	cout << "5. FAT32文件位置矫正    6. 异或加减&左旋&异或" << endl;
	cout << "7. 文件简单解密    8.jpg文件结构修复    9.doc文件头修复" << endl;
	cout << "10. 退出      能力有限, bug很多, 请谅解      " << endl;
	cout << "                                     联系QQ: 1762202060" << endl;
	cout << "-------------------------------------------------------" << endl;
	cout << "请选择功能 (对应编号)：";
	cin >> dec >> index;
	cout << "-------------------------------------------------------" << endl;
	return index;
}

void exit() {
	system("cls");
	cout << "-------------------------------------------------" << endl;
	cout << "　  　　　 ┏┓　 ┏┓          ┏┓　  ┏┓        " << endl;
	cout << "　  　　　┏┛┻━━━┻┗━━┓      ┏┛┻━━━━┛┻━┓      " << endl;
	cout << "　  　　　┃　　　　 ┃      ┃　　　　 ┃      " << endl;
	cout << "　  　　　┃　　　━　┃      ┃   ━　   ┃      " << endl;
	cout << "　  　　　┃　┳┛　┗┳ ┃      ┃　┳┛　┗┳ ┃      " << endl;
	cout << "　  　　　┃　　　　 ┃      ┃　　　　 ┃      " << endl;
	cout << "　  　　　┃　　　┻  ┃      ┃　 ┻　 　┃      " << endl;
	cout << "　  　　　┗━┓　　┏━━┛      ┗━┓　　┏━━┛      " << endl;
	cout << "　  　　　　┃　　┃           ┃　　┃　       " << endl;
	cout << "            ┃    ┃           ┃　　┃         " << endl;
	cout << "　  　  ┏━━━┛　　┃           ┃　  ┗━━┓      " << endl;
	cout << "　     ┏┫　　　　┃  神兽护体 ┃　　 　┣┓     " << endl;
	cout << "　     ┗┓　   　 ┃  1.5 版本 ┃　　 　┏┛     " << endl;
	cout << "　  　  ┗┓┏┳━━┓┏┏┛           ┗┓┓┏━┳┓┏┛      " << endl;
	cout << "　  　　 ┣┣┃　┣┣┃             ┃┫┫ ┃┫┫       " << endl;
	cout << "　  　　 ┗┻┛　┗┻┛             ┗┻┛ ┗┻┛       " << endl;
	cout << "-------------------------------------------------" << endl;
	cout << "                                       作者：杨鑫" << endl;
	system("pause");
}
// 神兽护体

int main(void) {
	FAT32* fat32 = new FAT32(); // 定义FAT32对象
	NTFS* ntfs = new NTFS(); // 定义NTFS对象
	exFAT* exfat = new exFAT(); // 定义exFAT对象
	EXT* ext = new EXT(); // 定义EXT对象

	vector<uint16_t> Cipher; // 定义 异或和左旋通用的 密文数组
	vector<uint16_t> Plaintext; // 定义 异或和左旋通用的 明文数组

	string filename; // 文件名, 判断文件类型
	vector<uint16_t> fileContent; // 定义 文件内容 数组 （大容量）
	int index;

	while (index = Choose(index)) {
		if (index == 10) break;
		switch (index) {
		case 1:
			system("cls");
			Template_FAT32();
			Traverse_FAT32(*fat32);
			system("pause"), system("cls");
			break;
		case 2:
			system("cls");
			Template_NTFS();
			Traverse_NTFS(*ntfs);
			system("pause"), system("cls");
			break;
		case 3:
			system("cls");
			Template_exFAT();
			Traverse_exFAT(*exfat);
			system("pause"), system("cls");
			break;
		case 4:
			system("cls");
			Template_EXT();
			Traverse_EXT(*ext);
			system("pause"), system("cls");
			break;
		case 5:
			system("cls");
			Correct_FAT32(fat32->Cu);
			system("pause"), system("cls");
			break;
		case 6:
			system("cls");
			setCipher(Cipher);
			if (!setPlaintext(Plaintext, Cipher)) {
				Cipher.clear(), Plaintext.clear();
				exit(); system("cls"); break;
			}
			xor_and_dec(Plaintext, Cipher); // 进行异或加减解密(xor and dec)
			add_and_xor(Plaintext, Cipher); // 进行加减异或解密(add and xor)
			Levo(Plaintext, Cipher);
			Xor(Plaintext, Cipher);
			Cipher.clear(), Plaintext.clear();
			system("pause"), system("cls");
			break;
		case 7:
			system("cls");
			cout << "如果rtf文档的后缀被改为doc, 请改回rtf" << endl;
			fileContent = ReadFile(filename);
			Choicefile(filename, fileContent);
			system("pause"), system("cls");
			break;
		case 8:
			system("cls");
			jpgStructure();
			system("pause"), system("cls");
			break;
		case 9:
			system("cls");
			docStructure();
			system("pause"), system("cls");
			break;
		default:
			cout << "输入错误" << endl;
			system("pause");
			exit();
			system("cls");
			break;
		}
	}

	delete fat32, ntfs, exfat, ext;
	exit();
	return 0;
}