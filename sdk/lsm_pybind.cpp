#include "../include/lsm/engine.h"
#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

// 绑定 TwoMergeIterator 迭代器
void bind_TwoMergeIterator(py::module &m) {
  py::class_<TwoMergeIterator>(m, "TwoMergeIterator")
      .def("__iter__", [](TwoMergeIterator &it) { return &it; })
      .def("__next__", [](TwoMergeIterator &it) {
        if (!it.is_valid())
          throw py::stop_iteration();
        auto kv = *it;
        ++it;
        return py::make_tuple(kv.first, kv.second);
      });
}

// 绑定 TranContext 事务上下文

// 提前声明 TranContext（如果头文件未包含）
class TranContext;

// 绑定 TranContext
void bind_TranContext(py::module &m) {
  py::class_<TranContext, std::shared_ptr<TranContext>>(m, "TranContext")
      .def("commit", &TranContext::commit,
           py::arg("test_fail") = false) // 处理默认参数
      .def("abort", &TranContext::abort)
      .def("get", &TranContext::get)
      .def("remove", &TranContext::remove)
      .def("put", &TranContext::put);
}

PYBIND11_MODULE(lsm_pybind, m) {
  // 绑定辅助类
  bind_TwoMergeIterator(m);
  bind_TranContext(m);

  // 主类 LSM
  py::class_<LSM>(m, "LSM")
      .def(py::init<const std::string &>())
      // 基础操作
      .def("put", &LSM::put, py::arg("key"), py::arg("value"),
           "Insert a key-value pair (bytes type)")
      .def("get", &LSM::get, py::arg("key"),
           "Get value by key, returns None if not found")
      .def("remove", &LSM::remove, py::arg("key"), "Delete a key")
      // 批量操作
      .def("put_batch", &LSM::put_batch, py::arg("kvs"),
           "Batch insert key-value pairs")
      .def("remove_batch", &LSM::remove_batch, py::arg("keys"),
           "Batch delete keys")
      // 迭代器
      .def("begin", &LSM::begin, py::arg("tranc_id"),
           "Start an iterator with transaction ID")
      .def("end", &LSM::end, "Get end iterator")
      // 事务
      .def("begin_tran", &LSM::begin_tran, "Start a transaction")
      // 其他方法
      .def("clear", &LSM::clear, "Clear all data") // ! Fix bugs
      .def("flush", &LSM::flush, "Flush memory table to disk")
      .def("flush_all", &LSM::flush_all, "Flush all pending data");
}