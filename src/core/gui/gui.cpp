#include "gui.hpp"

void Gui::start() {
        using namespace nana;
                 // Define widgets
        form fm;
        textbox usr  {fm};
        button  login {fm, "Open"}, 
                cancel{fm, "Exit"};
        
        usr.tip_string("Project file path:"    ).multi_lines(false);
                // Define a place for the form.
        place plc {fm};
                // Divide the form into fields
                
        //plc.div("margin= 10%   gap=20 vertical< weight=70 gap=20 vertical textboxs arrange=[25,25]> <min=20> <weight=25 gap=10 buttons>  > ");
        plc.div("<><weight=80% vertical<><weight=70% vertical <vertical gap=10 textboxs arrange=[25,25]>  <weight=25 gap=10 buttons> ><>><>");
        //Insert widgets
        //The field textboxs is vertical, it automatically adjusts the widgets' top and height. 
        plc.field("textboxs")<< usr;
        plc.field("buttons") << login << cancel;
        std::cout << usr.text() << std::endl;
        login.events().click([&]{
            msgbox(fm, usr.text()).show();
        });
        // Finially, the widgets should be collocated.
        // Do not miss this line, otherwise the widgets are not collocated
        // until the form is resized.
        plc.collocate();
        fm.show();
        exec();
}