compiler = g++
tram-sdk = ../tram-sdk/src/
tram-sdk-lib = ../tram-sdk/libraries/

src_init.o: ./src/init.cpp
	$(compiler) -c -O0 -std=c++20 -I$(tram-sdk) -I$(tram-sdk-lib) -Ilibraries -I./src ./src/init.cpp -o src_init.o

src_main.o: ./src/main.cpp
	$(compiler) -c -O0 -std=c++20 -I$(tram-sdk) -I$(tram-sdk-lib) -Ilibraries -I./src ./src/main.cpp -o src_main.o

src_tramsdk.o: ./src/tramsdk.cpp
	$(compiler) -c -O0 -std=c++20 -I$(tram-sdk) -I$(tram-sdk-lib) -Ilibraries -I./src ./src/tramsdk.cpp -o src_tramsdk.o

src_editor_editor.o: ./src/editor/editor.cpp
	$(compiler) -c -O0 -std=c++20 -I$(tram-sdk) -I$(tram-sdk-lib) -Ilibraries -I./src ./src/editor/editor.cpp -o src_editor_editor.o

src_editor_language.o: ./src/editor/language.cpp
	$(compiler) -c -O0 -std=c++20 -I$(tram-sdk) -I$(tram-sdk-lib) -Ilibraries -I./src ./src/editor/language.cpp -o src_editor_language.o

src_widgets_mainframe.o: ./src/widgets/mainframe.cpp
	$(compiler) -c -O0 -std=c++20 -I$(tram-sdk) -I$(tram-sdk-lib) -Ilibraries -I./src ./src/widgets/mainframe.cpp -o src_widgets_mainframe.o

src_widgets_objectlist.o: ./src/widgets/objectlist.cpp
	$(compiler) -c -O0 -std=c++20 -I$(tram-sdk) -I$(tram-sdk-lib) -Ilibraries -I./src ./src/widgets/objectlist.cpp -o src_widgets_objectlist.o

src_widgets_objectmenu.o: ./src/widgets/objectmenu.cpp
	$(compiler) -c -O0 -std=c++20 -I$(tram-sdk) -I$(tram-sdk-lib) -Ilibraries -I./src ./src/widgets/objectmenu.cpp -o src_widgets_objectmenu.o

src_widgets_propertypanel.o: ./src/widgets/propertypanel.cpp
	$(compiler) -c -O0 -std=c++20 -I$(tram-sdk) -I$(tram-sdk-lib) -Ilibraries -I./src ./src/widgets/propertypanel.cpp -o src_widgets_propertypanel.o

src_widgets_viewport.o: ./src/widgets/viewport.cpp
	$(compiler) -c -O0 -std=c++20 -I$(tram-sdk) -I$(tram-sdk-lib) -Ilibraries -I./src ./src/widgets/viewport.cpp -o src_widgets_viewport.o

src_widgets_worldtree.o: ./src/widgets/worldtree.cpp
	$(compiler) -c -O0 -std=c++20 -I$(tram-sdk) -I$(tram-sdk-lib) -Ilibraries -I./src ./src/widgets/worldtree.cpp -o src_widgets_worldtree.o

clean:
	del src_init.o
	del src_main.o
	del src_tramsdk.o
	del src_editor_editor.o
	del src_editor_language.o
	del src_widgets_mainframe.o
	del src_widgets_objectlist.o
	del src_widgets_objectmenu.o
	del src_widgets_propertypanel.o
	del src_widgets_viewport.o
	del src_widgets_worldtree.o

editor: src_init.o src_main.o src_tramsdk.o src_editor_editor.o src_editor_language.o src_widgets_mainframe.o src_widgets_objectlist.o src_widgets_objectmenu.o src_widgets_propertypanel.o src_widgets_viewport.o src_widgets_worldtree.o 
	$(compiler) -o trameditor -Llibraries/binaries/win64/ src_init.o src_main.o src_tramsdk.o src_editor_editor.o src_editor_language.o src_widgets_mainframe.o src_widgets_objectlist.o src_widgets_objectmenu.o src_widgets_propertypanel.o src_widgets_viewport.o src_widgets_worldtree.o -lwxmsw31u_gl -lwxmsw31u_propgrid -lwxmsw31u_aui -lwxmsw31u_xrc -lwxmsw31u_webview -lwxmsw31u_html -lwxmsw31u_adv -lwxmsw31u_core -lwxbase31u_xml -lwxbase31u_net -lwxbase31u
