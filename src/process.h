#ifndef Process_H
#define Process_H

#include <windows.h>

#include <node.h>
#include <node_object_wrap.h>

class Process : public node::ObjectWrap {
 public:
  static void Init(v8::Handle<v8::Object> exports);

 private:
  explicit Process(DWORD id = 0);
  ~Process();

  static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void Open(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void Read(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void Close(const v8::FunctionCallbackInfo<v8::Value>& args);
  static v8::Persistent<v8::Function> constructor;
  DWORD id_;
  HANDLE hdl_;
};

#endif
