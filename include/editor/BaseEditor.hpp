#ifndef _BASE_EDITOR_
#define _BASE_EDITOR_

namespace Guarneri
{
	class BaseEditor
	{
	public:
		BaseEditor();
		~BaseEditor();
		void open();
		void close();
		void render();
		virtual void on_gui() = 0;

	private:
		bool show;
	};
}

#endif