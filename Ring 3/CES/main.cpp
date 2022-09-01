
#include"Ceydbg.h"
using namespace std;

int Update_Flag = 0;
int Root = 0;//Root Ȩ��
DWORD  error;//error_code
CHAR Current_Path[MAX_PATH] = { 0 };
UCHAR Cin_Command[2048] = { 0 };//�����������



//Process incoming commands
int Process_Command(PUCHAR C) {
	DWORD64 Parameter = { 0 };
	Command Task = { 0 };
	int state = 0;
	int Cin_Parameter = 0;
	int Cin_Parameter_Offset = 0;

	//ͨ���ո�ָ����� Distinguish parameters by spaces
	for (int i = 0; C[i] != 0x00; i++)
	{
		if (C[i] == 0x20)
			continue;
		memcpy(&(Task.Parameter[Cin_Parameter][Cin_Parameter_Offset]), &C[i], 1);
		Cin_Parameter_Offset++;
		if (C[i + 1] == 0x20 || C[i + 1] == 0)
		{
			
			if (Cin_Parameter_Offset == 256)
				break;//Parameter too long

			//strcat((char*)&(Task.Parameter[Cin_Parameter]), "\n");

			Cin_Parameter++;
			Cin_Parameter_Offset = 0;
			if (Cin_Parameter == 10)
				break;//Command too long 
		}
	}
	Task.Parameter_Num = Cin_Parameter - 1;


	//�������� like windbg
	//�˴��ɸ��� renewable
	if (Task.Parameter[0][0] == 'd')
	{
		if (strcmp((char*)&(Task.Parameter[0]), "db") == 0)//ͬwindbg
			return Read_Memory(&Pdbfile, &Task, 1);

		if (strcmp((char*)&(Task.Parameter[0]), "dw") == 0)//ͬwindbg
			return Read_Memory(&Pdbfile, &Task, 2);

		if (strcmp((char*)&(Task.Parameter[0]), "dd") == 0)//ͬwindbg
			return Read_Memory(&Pdbfile, &Task, 4);

		if (strcmp((char*)&(Task.Parameter[0]), "dq") == 0)//ͬwindbg
			return Read_Memory(&Pdbfile, &Task, 8);
	}

	if (strcmp((char*)&(Task.Parameter[0]), "x") == 0)//ͬwindbg
		return CheckPdb_X((char*)&(Task.Parameter[1]));

	//�˳� quit
	if (strcmp((char*)&(Task.Parameter[0]), "quit") == 0)
	{
		CloseHandle(Pdbfile.My_Drive);
		if (!Stop_Drive())
			Error_Out(0);
		if (!Unload_Drive())
			Error_Out(0);
		return -1;
	}

	//if (strcmp((char*)&(Task.Parameter[0]), "root") == 0)
	//{
	//	Root = 1;
	//	printf("\n Warning : ROOT\n");
	//	return 1;
	//}

	return state;
}


int main() {

	//�����Ƿ��Ѿ���װ
	Pdbfile.My_Drive = CreateFile(DEVICE_NAME, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_SYSTEM, NULL);
	if (Pdbfile.My_Drive != INVALID_HANDLE_VALUE)
	{
		CloseHandle(Pdbfile.My_Drive);
		memset(&(Pdbfile.My_Drive), 0, sizeof(HANDLE));
	}
	else
	{
		//Install_Drive
		memset(&(Pdbfile.My_Drive), 0, sizeof(HANDLE));
		if (!Install_Drive())
			Error_Out(0);
		if (!Start_Drive())
			Error_Out(0);
	}
	
	//�ж��Ƿ�������pdb�ļ���û�о�����
	Get_Ntoskrnl_Pdb();
	//����pdb�ļ���exe
	Load_Pdb();
	//��������
	Load_Drive();



	int State = 1;
	while (1) {
		rewind(stdin);
		State = 1;

		memset(&Cin_Command, 0, 2048);
		printf("\nic>");
		scanf_s("%[^\n]", Cin_Command, 2048);

		State = Process_Command(Cin_Command);
		if (State == 0)
			printf(" Command Error!\n");
		if (State == -1)
			return 0;
	}
    system("pause");

}