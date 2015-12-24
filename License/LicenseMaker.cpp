// License.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "License.h"

int _tmain(int argc, _TCHAR* argv[])
{
	cout << "=====��Ȩ���ɹ���v1.0=====" << endl;
	cout << endl;
	cout << "!!!���ĳ��ȫ��ʿһ�������������档�����������޷�¼��!!!" << endl;
	cout << endl;

	// 1.���ɹ�˽Կ
	// 2.ʹ������˽Կǩ���ļ�
	char priKey[128] = { 0 };
	char pubKey[128] = { 0 };
	char seed[1024] = { 0 };

	// ���� RSA ��Կ��
	strcpy(priKey, "1.PrivateKey");  // ���ɵ�˽Կ�ļ�
	strcpy(pubKey, "1.PublicKey");  // ���ɵĹ�Կ�ļ�
	strcpy(seed, "seed");
	GenerateRSAKey(1024, priKey, pubKey, seed);

	// RSA �ӽ���
	char message[1024] = { 0 };
	cout << "Origin Text:\t" << "Hello World!" << endl << endl;
	strcpy(message, "Hello World!");
	string encryptedText = RSAEncryptString(pubKey, seed, message);  // RSA ����
	cout << "Encrypted Text:\t" << encryptedText << endl << endl;
	string decryptedText = RSADecryptString(priKey, encryptedText.c_str());  // RSA ����
	//string decryptedText = RSADecryptString(priKey, "6014655DB01F43C33B292ADB2AD6BA567EF156BD8380CC1C3C79987523BB9416EE87970CA99BE79E958D0AF3FB5B17F43A52E12EEDDD3B23C07FD2EE4C3D9ACFBE10BD9B4B752B1ADD738ED74E9ED76650AB8ED76E76767F77DD311F88BCD2B15E4C106A552DED2D241910F1EDF881095DF34F8CDA16B502BF51FF3DE56FC37E");  // RSA ����
	cout << "Decrypted Text:\t" << decryptedText << endl << endl;

	RSASignFile(priKey, priKey, "a");
	bool  b = RSAVerifyFile(pubKey,priKey,"a");

	return 0;
}

