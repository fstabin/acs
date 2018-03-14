#pragma once
#include <comdef.h>
#include <MsXml6.h> 
#include <atlutil.h>
#include <functional>
#include <stack>
#include <algorithm>
#pragma comment(lib, "MsXml6.lib")

#define HR_CHECK(hr) if(FAILED(hr))return false;

namespace acs {
	namespace xml {
		//SAXハンドラクラスサポートクラス
		class SaxContentHandler :public ISAXContentHandler {
			ULONG refcnt = 1;
		public:
			virtual HRESULT STDMETHODCALLTYPE QueryInterface(
				/* [in] */ REFIID riid,
				/* [iid_is][out] */ _COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject) {
				if (__uuidof(ISAXContentHandler) == riid) {
					*ppvObject = static_cast<ISAXContentHandler*>(this);
					++refcnt;
					return S_OK;
				}
				*ppvObject = nullptr;
				return E_FAIL;
			}

			ULONG STDMETHODCALLTYPE AddRef(void) {
				refcnt++;
				return refcnt;
			}
			ULONG STDMETHODCALLTYPE Release(void) {
				refcnt--;
				if (refcnt == 0)delete this;
				return refcnt;
			}
		};

		class XmlSax {
			HRESULT last_result;
			CComPtr<ISAXXMLReader>  pXMLSax = nullptr;
			bool SAX_is_ok = false;
		public:
			XmlSax() {
				CComPtr<ISAXXMLReader>  __pXMLSax = nullptr;
				last_result = ::CoCreateInstance(
					CLSID_SAXXMLReader60,
					NULL,
					CLSCTX_INPROC_SERVER,
					IID_ISAXXMLReader,
					(void**)&__pXMLSax
				);
				if (FAILED(last_result)) {
					SAX_is_ok = false;
				}
				else {
					pXMLSax = __pXMLSax;
					SAX_is_ok = true;
				}
			}
			XmlSax(XmlSax&) = delete;
			XmlSax& operator=(XmlSax&) = delete;
			bool is_ok() {
				return (SAX_is_ok);
			}
			bool setContentHandle(ISAXContentHandler* handle) {
				last_result = pXMLSax->putContentHandler(handle);
				if (FAILED(last_result)) {
					return false;
				}
				else {
					return true;
				}
			}
			//成功で設定されたハンドルのポインタ,失敗でnull
			bool getContentHandle(ISAXContentHandler** pphandle) {
				ISAXContentHandler* hnd = nullptr;
				last_result = pXMLSax->getContentHandler(&hnd);
				*pphandle = hnd;
				return SUCCEEDED(last_result);
			}


			//XML展開(検索パスは、[ファイルパスそのまま->カレントディレクトリ] の順)
			bool parseXml(const wchar_t* filepass) {
				if (SAX_is_ok == false)return false;
				HRESULT hr;
				hr = pXMLSax->parseURL(filepass);
				if (FAILED(hr)) {
					wchar_t szCurrent[_MAX_PATH];
					::GetCurrentDirectoryW(_MAX_PATH, szCurrent);
					wchar_t szFull[_MAX_PATH];
					_wmakepath_s(szFull, _MAX_PATH, nullptr, szCurrent, filepass, nullptr);
					hr = pXMLSax->parseURL(szFull);
				}
				last_result = hr;
				return (SUCCEEDED(hr));
			}
		};

		class XmlDomText;
		class XmlDomElement;
		class XmlDom;

		struct CB_ChildFind {
			virtual bool Onfind(XmlDomElement* fount) = 0;//true で続行　false で中断
		};

		class XmlDomText {
			CComPtr<IXMLDOMNode>  __pNode = nullptr;
		public:
			XmlDomText() {
			}
			XmlDomText(XmlDomText&) = delete;
			XmlDomText& operator=(XmlDomText&) = delete;

			class _AllowTextSet {
		protected:
			inline bool __reset(XmlDomText*elm, IXMLDOMNode* _newnode) {
				elm->__pNode = _newnode;
				return true;
			}
			
		};

			bool get(const wchar_t** re_text) {
				VARIANT var;
				HR_CHECK(__pNode->get_nodeValue(&var));
				if (var.vt != VT_BSTR)return false;
				*re_text = var.bstrVal;
				return true;
			}
		};

		class XmlDomElement :XmlDomText::_AllowTextSet {
			HRESULT last_result;
			bool ok = false;
			CComPtr<IXMLDOMNode>  __pNode= nullptr;
			CComPtr<IXMLDOMNodeList>  __pChildList = nullptr;

			bool reset(IXMLDOMNode* dom) {
				__pNode = dom;
				if (__pChildList)__pChildList.Release();
				HR_CHECK(__pNode->get_childNodes(&__pChildList));
				return true;
			}
		public:
			class _AllowElementSet {
			protected:
				inline bool __resetElmptr(XmlDomElement*elm,IXMLDOMNode* dom) {
					return elm->reset(dom);
				}
			};
			XmlDomElement() {
			}
			XmlDomElement(XmlDomElement&) = delete;
			XmlDomElement& operator=(XmlDomElement&) = delete;

			bool get_attribute(wchar_t* name, wchar_t** outstr) {
				CComPtr<IXMLDOMNamedNodeMap>  nnm = nullptr;
				CComPtr<IXMLDOMNode>  node = nullptr;
				HR_CHECK(__pNode->get_attributes(&nnm));
				HR_CHECK(nnm->getNamedItem(name, &node));
				VARIANT var;
				HR_CHECK(node->get_nodeValue(&var));
				*outstr = var.bstrVal;
				return true;
			}
			bool get_tagName(wchar_t** name) {
				return SUCCEEDED(__pNode->get_nodeName(name));
			}
			bool get_text(unsigned int index, XmlDomText* text) {
				long length = 0;
				HR_CHECK(__pChildList->get_length(&length));
				wchar_t* tag_name = nullptr;
				unsigned int textnode_cnt = 0;
				for (long i = 0; i < length; i++)
				{
					CComPtr<IXMLDOMNode>Child = nullptr;
					HR_CHECK(__pChildList->get_item(i, &Child));
					DOMNodeType dnt;
					HR_CHECK(Child->get_nodeType(&dnt));
					if (dnt == NODE_TEXT) {
						if (textnode_cnt == index) {
							_AllowTextSet::__reset(text, Child);
							return true;
						}
						else textnode_cnt++;
					}
				}
				return false;
			}
			bool get_childelm(const wchar_t* name,XmlDomElement* result) {
				long length = 0;
				HR_CHECK(__pChildList->get_length(&length));
				wchar_t* tag_name = nullptr;
				for (long i = 0; i < length; i++)
				{
					CComPtr<IXMLDOMNode>Child = nullptr;
					HR_CHECK(__pChildList->get_item(i, &Child));
					DOMNodeType dnt;
					HR_CHECK(Child->get_nodeType(&dnt));
					if (dnt != NODE_ELEMENT)continue;
					HR_CHECK(Child->get_nodeName(&tag_name));
					if (StrCmpCW(tag_name, name) == 0) {
						result->reset(Child);
						return true;
					}
				}
				return false;
			}
			
			//func bool型
			//[&](XmlDomElement*)->bool{}
			bool get_childelms(const wchar_t* name, std::function<bool(XmlDomElement*)> out_bool_in_const_wchar_t_ptr_func) {
				long length = 0;
				HR_CHECK(__pChildList->get_length(&length));
				wchar_t* tag_name = nullptr;
				for (long i = 0; i < length; i++)
				{
					CComPtr<IXMLDOMNode>Child = nullptr;
					HR_CHECK(__pChildList->get_item(i, &Child));
					DOMNodeType dnt;
					HR_CHECK(Child->get_nodeType(&dnt));
					if (dnt != NODE_ELEMENT)continue;
					HR_CHECK(Child->get_nodeName(&tag_name));
					if (StrCmpCW(tag_name, name) == 0) {
						XmlDomElement elm;
						elm.reset(Child);
						if (out_bool_in_const_wchar_t_ptr_func(&elm))continue;
						return true;
					}
				}
				return false;
			}
			
			//func 返り値bool 第一引数にconst wchar_t* 型
			//[&](const wchar_t* data)->void{}
			bool get_child_text(const wchar_t* childName, std::function<void(const wchar_t*)> func)
			{
				XmlDomElement __elm;
				XmlDomText data;
				if (this->get_childelm(childName, &__elm)) {
					const wchar_t* mtext;
					if (__elm.get_text(0, &data)) {
						if (data.get(&mtext)) {
							func(mtext);
							return true;
						}
					}
				}
				return false;
			}
		};

		class XmlDom  :XmlDomElement::_AllowElementSet {
			bool ok = false;
			CComPtr<IXMLDOMDocument2> __pDom = nullptr;
			CComPtr<IXMLDOMNodeList>  __pNodes = nullptr;
		public:

			XmlDom() {
				HRESULT hr;
				hr = ::CoCreateInstance(
					CLSID_DOMDocument60,
					NULL,
					CLSCTX_INPROC_SERVER,
					IID_PPV_ARGS(&__pDom)
				);
				if (FAILED(hr))return;
				hr = __pDom->put_async(VARIANT_FALSE);
				if (FAILED(hr))return;
				hr = __pDom->put_validateOnParse(VARIANT_FALSE);
				if (FAILED(hr))return;
				hr = __pDom->put_resolveExternals(VARIANT_FALSE);
				if (FAILED(hr))return;
				ok = true;
			}
			XmlDom(XmlDom&) = delete;
			XmlDom& operator=(XmlDom&) = delete;
			bool is_ok() {
				return (ok);
			}
			//filepass NULLで終わる文字列
			bool loadXml(const wchar_t* filepass) {
				if (ok == false)return false;
				HRESULT hr;
				VARIANT variant;
				variant.vt = VT_BSTR;
				CComBSTR filep(filepass);
				variant.bstrVal = filep;
				VARIANT_BOOL vb;
				{
					hr = __pDom->load(variant,&vb);
				}
				if (hr != S_OK) {
					wchar_t szCurrent[_MAX_PATH];
					::GetCurrentDirectoryW(_MAX_PATH, szCurrent);
					wchar_t szFull[_MAX_PATH];
					_wmakepath_s(szFull, _MAX_PATH, nullptr, szCurrent, filepass, nullptr);
					filep = CComBSTR(szFull);
					variant.bstrVal = filep;
					hr = __pDom->load(variant, &vb);
				}
				if (hr != S_OK) return false;
				hr = __pDom->get_childNodes(&__pNodes);
				if (hr != S_OK) return false;
				return true;
			}
			bool get_childelm(const wchar_t* name, XmlDomElement* result) {
				long length;
				HR_CHECK(__pNodes->get_length(&length));
				wchar_t* tag_name = nullptr;
				for (long i = 0; i < length; i++)
				{
					CComPtr<IXMLDOMNode>Child = nullptr;
					HR_CHECK(__pNodes->get_item(i, &Child));
					DOMNodeType dnt;
					HR_CHECK(Child->get_nodeType(&dnt));
					if (dnt != NODE_ELEMENT)continue;
					HR_CHECK(Child->get_baseName(&tag_name));
					if (StrCmpCW(tag_name, name) == 0) {
						_AllowElementSet::__resetElmptr(result, Child);
						return true;
					}
				}
				return false;
			}
		};

		class XmlReader {
			bool ok = false;
			CComPtr<IXMLDOMDocument2> __pDom = nullptr;
			CComPtr<IXMLDOMNodeList>  __pNodes = nullptr;
			std::stack<IXMLDOMNode*> stack;
			std::stack<IXMLDOMNodeList*> stack1;
		public:
			XmlReader() {
				HRESULT hr;
				hr = ::CoCreateInstance(
					CLSID_DOMDocument60,
					NULL,
					CLSCTX_INPROC_SERVER,
					IID_PPV_ARGS(&__pDom)
				);
				if (FAILED(hr))return;
				hr = __pDom->put_async(VARIANT_FALSE);
				if (FAILED(hr))return;
				hr = __pDom->put_validateOnParse(VARIANT_FALSE);
				if (FAILED(hr))return;
				hr = __pDom->put_resolveExternals(VARIANT_FALSE);
				if (FAILED(hr))return;
				ok = true;
			}
			XmlReader(const wchar_t* filepass) {
				HRESULT hr;
				hr = ::CoCreateInstance(
					CLSID_DOMDocument60,
					NULL,
					CLSCTX_INPROC_SERVER,
					IID_PPV_ARGS(&__pDom)
				);
				if (FAILED(hr))return;
				hr = __pDom->put_async(VARIANT_FALSE);
				if (FAILED(hr))return;
				hr = __pDom->put_validateOnParse(VARIANT_FALSE);
				if (FAILED(hr))return;
				hr = __pDom->put_resolveExternals(VARIANT_FALSE);
				if (FAILED(hr))return;
				ok = true;
				ok = loadXml(filepass);
			}
			~XmlReader() {
				set_depth(0);
			}
	
			bool is_ok() {
				return (ok);
			}

			bool loadXml(const wchar_t* filepass) {
				if (ok == false)return false;
				HRESULT hr;
				VARIANT variant;
				variant.vt = VT_BSTR;
				CComBSTR filep(filepass);
				variant.bstrVal = filep;
				VARIANT_BOOL vb;
				{
					hr = __pDom->load(variant, &vb);
				}
				if (hr != S_OK) {
					wchar_t szCurrent[_MAX_PATH];
					::GetCurrentDirectoryW(_MAX_PATH, szCurrent);
					wchar_t szFull[_MAX_PATH];
					_wmakepath_s(szFull, _MAX_PATH, nullptr, szCurrent, filepass, nullptr);
					filep = CComBSTR(szFull);
					variant.bstrVal = filep;
					hr = __pDom->load(variant, &vb);
				}
				if (hr != S_OK) return false;
				__pNodes.Release();
				hr = __pDom->get_childNodes(&__pNodes);
				if (hr != S_OK) return false;
				return true;
			}

			void set_top() {
				set_depth(0);
			}
			void set_depth(size_t depth) {
				while (stack.size() > depth) {
					stack.top()->Release();
					stack.pop();
					stack1.top()->Release();
					stack1.pop();
				}
			}
			void goto_parent() {
				while (stack.size()) {
					stack.top()->Release();
					stack.pop();
				}
			}

			bool get_attribute(wchar_t* name, std::wstring& wstr) {
				if (ok == false)return false;
				IXMLDOMNode* pn = (stack.empty()) ? __pDom : stack.top();
				CComPtr<IXMLDOMNamedNodeMap>  nnm = nullptr;
				CComPtr<IXMLDOMNode>  node = nullptr;
				HR_CHECK(pn->get_attributes(&nnm));
				HR_CHECK(nnm->getNamedItem(name, &node));
				VARIANT var;
				HR_CHECK(node->get_nodeValue(&var));
				wstr = var.bstrVal;
				return true;
			}
			bool get_tagName(wchar_t** name) {
				if (ok == false)return false;
				IXMLDOMNode* pn = (stack.empty()) ? __pDom : stack.top();
				return SUCCEEDED(pn->get_nodeName(name));
			}
			bool get_text(unsigned int index, std::wstring& wstr) {
				if (ok == false)return false;
				IXMLDOMNodeList* pn = (stack1.empty()) ? __pNodes : stack1.top();
				long length = 0;
				HR_CHECK(pn->get_length(&length));
				wchar_t* tag_name = nullptr;
				unsigned int textnode_cnt = 0;
				for (long i = 0; i < length; i++)
				{
					CComPtr<IXMLDOMNode>Child = nullptr;
					HR_CHECK(pn->get_item(i, &Child));
					DOMNodeType dnt;
					HR_CHECK(Child->get_nodeType(&dnt));
					if (dnt == NODE_TEXT) {
						if (textnode_cnt == index) {
							VARIANT var;
							HR_CHECK(Child->get_nodeValue(&var));
							if (var.vt != VT_BSTR)return false;
							wstr.clear();
							wstr += var.bstrVal;
							return true;
						}
						else textnode_cnt++;
					}
				}
				return false;
			}
			bool goto_child(const wchar_t* name, size_t index = 0) {
				if (ok == false)return false;
				IXMLDOMNodeList* pn = (stack1.empty()) ? __pNodes : stack1.top();
				long length;
				HR_CHECK(pn->get_length(&length));
				wchar_t* tag_name = nullptr;
				size_t found = 0;
				for (long i = 0; i < length; i++)
				{
					CComQIPtr<IXMLDOMNode>Child = nullptr;
					HR_CHECK(pn->get_item(i, &Child));
					DOMNodeType dnt;
					HR_CHECK(Child->get_nodeType(&dnt));
					if (dnt != NODE_ELEMENT)continue;
					HR_CHECK(Child->get_baseName(&tag_name));
					if (StrCmpCW(tag_name, name) == 0) {
						if (found == index) {
							IXMLDOMNode* p;
							Child.CopyTo(&p);
							stack.push(p);
							IXMLDOMNodeList* p1;
							HR_CHECK(p->get_childNodes(&p1));
							stack1.push(p1);
							return true;
						}
						found++;
					}
				}
				return false;
			}
			ACS_NO_COPY(XmlReader);
		};
	}
}

#undef HR_CHECK