#include <windows.h>
#include <tlhelp32.h>

#include <node.h>
#include "process.h"

using namespace v8;

void GetProcessByName(const FunctionCallbackInfo<Value>& args)
{
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);
 
    if (args.Length() < 1)
    {
        isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8(isolate, "Wrong number of arguments")));
        return;
    }

    if (!args[0]->IsString())
    {
        isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8(isolate, "Wrong arguments")));
        return;
    }

    String::Utf8Value procName(args[0]->ToString());

    HANDLE hdl = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);;

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    BOOL rval;
    for (rval = Process32First(hdl, &pe32);
         rval == TRUE && strcmpi(pe32.szExeFile, *procName) != 0;
         rval = Process32Next(hdl, &pe32));

    if (hdl != INVALID_HANDLE_VALUE)
    {
        CloseHandle(hdl);
    }

    Local<Number> num;
    if (rval == TRUE)
    {
        num = Number::New(isolate, pe32.th32ProcessID);
    }
    else
    {
        num = Number::New(isolate, 0);
    }

    args.GetReturnValue().Set(num);
}

void InitAll(Handle<Object> exports) {
  Process::Init(exports);

  NODE_SET_METHOD(exports, "getProcessByName", GetProcessByName);

}

NODE_MODULE(rpm, InitAll)
