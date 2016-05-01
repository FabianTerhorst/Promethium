#include "Process.h"

using namespace v8;

BOOL EnableDebugPrivileges()
{
    HANDLE token;
    LUID sedebugnameValue;
    TOKEN_PRIVILEGES privileges;
 
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token))
    {
        if (LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &sedebugnameValue))
        {
            privileges.PrivilegeCount = 1;
            privileges.Privileges[0].Luid = sedebugnameValue;
            privileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

            if (AdjustTokenPrivileges(token, FALSE, &privileges, sizeof(privileges), NULL, NULL))
            {
                CloseHandle(token);
                return TRUE;
            }
        }
        CloseHandle(token);
    }

    return FALSE;
}

Persistent<Function> Process::constructor;

Process::Process(DWORD id) : id_(id), hdl_(INVALID_HANDLE_VALUE) {
}

Process::~Process() {
}

void Process::Init(Handle<Object> exports) {
  Isolate* isolate = Isolate::GetCurrent();

  // Prepare constructor template
  Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
  tpl->SetClassName(String::NewFromUtf8(isolate, "Process"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  // Prototype
  NODE_SET_PROTOTYPE_METHOD(tpl, "open", Open);
  NODE_SET_PROTOTYPE_METHOD(tpl, "read", Read);
  NODE_SET_PROTOTYPE_METHOD(tpl, "close", Close);

  constructor.Reset(isolate, tpl->GetFunction());
  exports->Set(String::NewFromUtf8(isolate, "Process"),
               tpl->GetFunction());
}

void Process::New(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  if (args.IsConstructCall()) {
    // Invoked as constructor: `new Process(...)`
    DWORD id = args[0]->IsUndefined() ? 0 : (DWORD)args[0]->NumberValue();
    Process* obj = new Process(id);
    obj->Wrap(args.This());
    args.GetReturnValue().Set(args.This());
  } else {
    // Invoked as plain function `Process(...)`, turn into construct call.
    const int argc = 1;
    Local<Value> argv[argc] = { args[0] };
    Local<Function> cons = Local<Function>::New(isolate, constructor);
    args.GetReturnValue().Set(cons->NewInstance(argc, argv));
  }
}

void Process::Open(const FunctionCallbackInfo<Value>& args)
{
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    Process* obj = ObjectWrap::Unwrap<Process>(args.Holder());

    EnableDebugPrivileges();

    obj->hdl_ = OpenProcess(PROCESS_VM_READ, FALSE, obj->id_);

    args.GetReturnValue().Set(Number::New(isolate, (uint64_t)obj->hdl_));
}

void Process::Read(const FunctionCallbackInfo<Value>& args)
{
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    if (args.Length() < 2)
    {
        isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8(isolate, "Wrong number of arguments")));
        return;
    }

    if (!args[0]->IsNumber() || !args[1]->IsNumber())
    {
        isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8(isolate, "Wrong arguments")));
        return;
    }

    DWORD_PTR address = args[0]->ToNumber()->Value();
    SIZE_T size = (SIZE_T)(args[1]->ToNumber()->Value() < 4 ? args[1]->ToNumber()->Value() : 4);

    Process* obj = ObjectWrap::Unwrap<Process>(args.Holder());

    DWORD value = 0;
    SIZE_T bytes = 0;
    BOOL rval = ReadProcessMemory(obj->hdl_, (LPCVOID)address, &value, size, &bytes);

    Local<Number> num;
    if (rval == TRUE)
    {
        num = Number::New(isolate, value);
    }
    else
    {
        num = Number::New(isolate, GetLastError());
    }

    args.GetReturnValue().Set(num);
}

void Process::Close(const FunctionCallbackInfo<Value>& args)
{
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    Process* obj = ObjectWrap::Unwrap<Process>(args.Holder());

    if (obj->hdl_ != INVALID_HANDLE_VALUE)
    {
        CloseHandle(obj->hdl_);
    }
}
