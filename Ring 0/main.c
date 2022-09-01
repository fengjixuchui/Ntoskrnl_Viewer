#include"CeyDbg.h"

PDEVICE_OBJECT mydevice = NULL;
NTSTATUS Unload(PDRIVER_OBJECT main) {
	DbgPrint("now exit!\n");
	UNICODE_STRING symbolLinkName;
	DbgPrint("DriverUnload\n");
	if (main->DeviceObject)
		IoDeleteDevice(main->DeviceObject);
	RtlInitUnicodeString(&symbolLinkName, my_link_name);
	IoDeleteSymbolicLink(&symbolLinkName);
}
NTSTATUS CreateCompleteRoutine(PDEVICE_OBJECT pDeviceObject, PIRP pIrp)
{
	NTSTATUS Status;

	DbgPrint("����\n");
	Status = STATUS_SUCCESS;
	pIrp->IoStatus.Status = Status;
	pIrp->IoStatus.Information = 0;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}
NTSTATUS myfun(PDEVICE_OBJECT mydevice, PIRP irp)
{
	
	NTSTATUS status = STATUS_SUCCESS;
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(irp);
	ULONG xh_code = stack->Parameters.DeviceIoControl.IoControlCode;
	ULONG buffer_lenth = 0;

	


	switch (xh_code)
	{
	case GET_BASE:
	{
		//DWORD64  k = &KeNumberProcessors;
		DWORD64 base = (DWORD64)KdDebuggerEnabled;
		irp->IoStatus.Status = status;
		PUCHAR buffer2 = stack->Parameters.DeviceIoControl.Type3InputBuffer;//����д��Ӧ��
		PUCHAR buffer3 = irp->UserBuffer;//���Դ�Ӧ�ö���
		//memcpy(&test, buffer3, sizeof(int));
		int test = 0;
		memcpy(buffer2, &base, sizeof(DWORD64));
		DbgPrint("get!:%d", test);
		irp->IoStatus.Information = 8;
		break;
	}
	case GET_VALUE:
	{
		//DbgBreakPoint();
		DWORD64 add = 0;
		irp->IoStatus.Status = status;
		PUCHAR buffer2 = stack->Parameters.DeviceIoControl.Type3InputBuffer;//����д��Ӧ��
		PUCHAR buffer3 = irp->UserBuffer;//���Դ�Ӧ�ö���
		memcpy(&add, buffer3, sizeof(DWORD64));
		ULONG il = stack->Parameters.DeviceIoControl.InputBufferLength;
		//ULONG ol = stack->Parameters.DeviceIoControl.OutputBufferLength;
		_try{
		memcpy(buffer2, add, il); 
		irp->IoStatus.Information = il;
		}
		_except(1) {
			//memset(buffer2, 0, il);
			irp->IoStatus.Information = 0;
		}
		
		break;
	}
	default:
		irp->IoStatus.Information = 0; break;
	}
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}
NTSTATUS DriverEntry(PDRIVER_OBJECT main, PUNICODE_STRING ei) {



	NTSTATUS status = STATUS_SUCCESS;
	UNICODE_STRING device_name, device_link_name;//�豸��������������
	RtlInitUnicodeString(&device_name, my_device_name);//��ʼ���豸��
	RtlInitUnicodeString(&device_link_name, my_link_name);//��ʼ������������
	status = IoCreateDevice(main, 200, &device_name, FILE_DEVICE_UNKNOWN, 0, 1, &mydevice);
	if (!NT_SUCCESS(status))
	{
		DbgPrint("�޷������豸!\n");
		return status;
	}
	status = IoCreateSymbolicLink(&device_link_name, &device_name);
	if (!NT_SUCCESS(status))
	{
		DbgPrint("�޷������������ӣ�\n");
		IoDeleteDevice(mydevice);
		return status;
	}
	DbgPrint("�����ɹ���\n");
	main->DriverUnload = Unload;
	main->MajorFunction[IRP_MJ_CREATE] = CreateCompleteRoutine;
	main->MajorFunction[IRP_MJ_DEVICE_CONTROL] = myfun;
}

