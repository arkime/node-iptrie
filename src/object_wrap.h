/* Minimal ObjectWrap, vendored from node_object_wrap.h (pre-24.19 semantics).
 *
 * Node 24.19.0 made node::ObjectWrap register an environment cleanup hook in
 * its constructor and remove it in its destructor. RemoveEnvironmentCleanupHook
 * asserts a current Environment, but ~ObjectWrap also runs from V8 GC weak
 * callbacks (GlobalHandles::InvokeFirstPassWeakCallbacks), where no context is
 * entered, so the assert aborts the process. IPTrie only needs Wrap/Unwrap.
 */
#ifndef SRC_OBJECT_WRAP_H_
#define SRC_OBJECT_WRAP_H_

#include <v8.h>
#include <cassert>

namespace arkime {

class ObjectWrap {
  public:
    ObjectWrap() : refs_(0) { }

    virtual ~ObjectWrap() {
      if (handle_.IsEmpty()) return;
      handle_.ClearWeak();
      handle_.Reset();
    }

    template <class T>
    static inline T *Unwrap(v8::Local<v8::Object> handle) {
      assert(!handle.IsEmpty());
      assert(handle->InternalFieldCount() > 0);
      void *ptr = handle->GetAlignedPointerFromInternalField(0);
      ObjectWrap *wrap = static_cast<ObjectWrap *>(ptr);
      return static_cast<T *>(wrap);
    }

    inline v8::Local<v8::Object> handle(v8::Isolate *isolate) {
      return v8::Local<v8::Object>::New(isolate, handle_);
    }

  protected:
    inline void Wrap(v8::Local<v8::Object> handle) {
      assert(handle_.IsEmpty());
      assert(handle->InternalFieldCount() > 0);
      handle->SetAlignedPointerInInternalField(0, this);
      handle_.Reset(v8::Isolate::GetCurrent(), handle);
      MakeWeak();
    }

    inline void MakeWeak() {
      handle_.SetWeak(this, WeakCallback, v8::WeakCallbackType::kParameter);
    }

    int refs_;

  private:
    static void WeakCallback(const v8::WeakCallbackInfo<ObjectWrap> &data) {
      ObjectWrap *wrap = data.GetParameter();
      assert(wrap->refs_ == 0);
      wrap->handle_.Reset();
      delete wrap;
    }

    v8::Persistent<v8::Object> handle_;
};

}  // namespace arkime

#endif  // SRC_OBJECT_WRAP_H_
