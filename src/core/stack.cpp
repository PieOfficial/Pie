#include <iostream> // header in standard library

using namespace std;



class start
{
private:
    /* data */
    const char *data = R""""(
                                                                                .^,;IllllII:^.                                                       
                                                                       .`":I!i!!!llIIIIIIIll!!ii;^                                                   
                                                                `,l!ii!!lIIIIIIIIIIIIIIIIIIIIIIII!il'                                                
                                                       '^:;l!!!lllIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIlil'                                              
                                               .',Iiiii!llIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII!i:                                             
                                        `,;l!ii!lllIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIii:                                            
                               .'`";!iii!!llIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIliii"                                           
                          ':l!iiii!lIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIlll!!!!iiiiiiiiiiii!.                                          
                   `";liiii!llIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIlll!iiiiiiiiiiiiiiiiiiiiiiiiiiiiii^                                          
             .^Iiiiii!lIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIl!iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii<,                                          
         ^;!iii!llIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIll!!iiiiiiiiiiiiiii!iii+]]][}{111))))))))11{[?~><~~"                                          
     .,!iiiiilIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIl!!iiiiiiiiiiiiiii><~_]{){1)))))))1111{11111111111{111{}-,                                          
   `!iiiiiilIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIl!!iiiiiiiiiiiiiii<-}11))))))))))111{111111111111111111111111111{~'                                       
 .!~iiiiiiilIIIIIIIIIIIIIIIIIIIIIIIIIll!iiiiiiiiiiiii>~_]{))))))))))1111{111111111111111111111111111111111111{<'                                     
.i~<iiiiiiiilIIIIIIIIIIIIIIIIIIIIl!iiiiiiiiiiiii>_{)))))))))){11{111{111{11111111111111111111111111111111111111{;                                    
!~~>iiiiiiiiii!lIIIIIIIIIIIIl!!iiiiiiiiiii<-[{)))))))))111{111111111111111111111111111111111111111111111111111111?`                                  
[~~<iiiiiiiiiiiiiii!!!!!iiiiiiiiiiii><-{)))))))))11111111{11111111111111111111111111111111111111111111111111111111{:                                 
1_~~iiiiiiiiiiiiiiiiiiiiiiiiiiii<?{1)))))))1111111111111111111111111111111111111111111111111111111111111111111111111~.                               
]]~~<iiiiiiiiiiiiiiiiiiiiii>+[))))))))111{11111111111111111111111111111111111111111111111111111111111111111111111111{_.                              
l)+~~~>iiiiiiiiiiiiiiiii~{)))))))1111{11111111111111111111111111111111111111111111111111111111111111111111111111111111[`                             
'])_~~~~<>iiiiiiiiiii_{)))))1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111[`                            
 I)1_~~~~~~~~<>>>><?))))1111{11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111{11,                           
  i))?~~~~~~~~~~~+{))1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111}:                          
   <))]~~~~~~~~~+1)1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111{:                         
   ._))[~~~~~~~~}1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111{I                        
     _))[~~~~~~+111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111{:                       
     .-))?~~~~~[|1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111I                      
      `{))_~~~~{xf111111111111111111111111111111111111111111111111111111111111111111111111111111111{11111111111111111111111111};                     
       ^))1+~~~1nunt)1{11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111{,                    
        >))?~~~1nunnunr|1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111{{;                   
        "{)[~~~}nuunnuuunnj|11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111},                  
        .?)1+~~?xuuunnnnnnnnnur|11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111{"                 
         ~))+~~+xuuuunnnnnnnnnnnnnrt)1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111}:                
         !))-~~~juuuuunnnnnnnnnnnnnununj(11111111111111111111111111111111111111111111111111111111111{111111111111111111111111111111{{"               
         I))?~~+fuuuuuuunnnnnnnnnnnnnnnnnunxf(11111{1111111111111111111111111111111111111111111111111111111111111111111111111111111111;              
         :))]~~+fuuuuuuuunnnnnnnnnnnnnnnnnuunnuuxf|)1111111111111111111111111111111111111111111111111111111111111111111111111111111111},             
         ,))]~~+tuuuuuuuuunnnnnnnnnnnnnnnnnnnnnnnuuunnx/1{1{1111111111111111111111111111111111111111111111111111111111111111111111111111,            
         ,))]~~~tuuuuuuuuuunnnnnnnnnnnnnnnnnnnnnnnnnnnnnnunrt|11111111111111111111111111111111111111111111111111111111111111111111111111{;           
         :))]~~~tuuuuuuuuuuunnnnnnnnnnnnnnnnnnnnnnnnnnnnnnuuunnunj(111111111111111111111111111111111111111111111111111111111111111111111{{"          
         ;))?~~~/vuuuuuuuuuuuunnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnunxrt)1{111111111111111111111111111111111111111111111111111111111111111{:         
         I))?~~~/vuuuuuuuuuuuuuuunnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnr/)111{111111111111111111111111111111111111111111111111111111111},        
         !))-~~~/vuuuuuuuuuuuuuuununnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnuuuunxt)111111111111111111111111111111111111111111111111111111111{`       
         i))_~~~|vuuuuuuuuuuuuuuvuuunnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnuuunrt(11111111111111111111111111111111111111111111111111111}"      
         <))_~~~|vuuuuuuuuuuuuuuuuuuununnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnunt111{1111111111111111111111111111111111111111111111]'     
         ~))+~~~(vuuuuuuuuuuuuuuuuuuuuunnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnuuuuunrf|1111111111111111111111111111111111111111111{1].    
         ~))+~~~(cuvuuuuuuuuuuuuuuuuuuuuununnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnunxf()1111111111111111111111111111111111111111_.   
         ~)1+~~~)zuuuuuuuuuuuuuuuuuuuuuuuuuuunnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnxj|1{{1111111111111111111111111111111111i   
         ~)1+~~~)zuuuuuuuuuuuuuuuuuuuuuuuuuuuunnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnuuunj/(11111111111111111111111111111111:  
         >))+~~~1Xvuuuuuuuuuuuuuuuuuuuuuuuuuvuunnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnuuuxt11{111111111111111111111111{{^ 
         !))_~~~{zcuuuuuuuuuuuuuuuuuuuuuuuuuuuuuunnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnxf|)1111111111111111111111+.
         :))-~~~[zXvuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuunnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnunuur/)11111111111111111{,
         .))]~~~-vXcuvuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuunnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnunxf(1111111111111-
          -)1~~~~fXXcuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuunnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnuuuununj/()111111}
          ;))?~~~~cYYXvuvuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuunnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnunuunnnnn|^
          .])1+~~~_rYYYXzvuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuunnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnuux, 
           I1)1+~~~~+fzYXXYXvvuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuunnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn:  
            l))){?~~~~~+]|vXXXXzcvuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuunnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn_   
             '+1)))){?+~~~~~~}fuzYXXcvvuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuununnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnt^   
                `!{))))){?~~~~~~~+-)nXXXXzvuuuuuuuuuuuuuuuuuuuuuuuuuuuuunnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn>    
                    '!-1)))){?_~~~~~~~~_{tnXYXzcvuuvuuuuuuuuuuuuuuuuuuuuuuuuunnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnuut     
                         `~1)))))]+~~~~~~~~~~+{xXXXXzvvuuuuuuuuuuuuuuuuuuuuuuuunnunnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnul     
                             `;+{)))1}?+~~~~~~~~~~_])rzXXzzcvuuuuuuuuuuuuuuuuuuuuuunnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnut`     
                                  '!?{))))[_+~~~~~~~~~~~~]tuzXXXcvvuuuuvuuuuuuuuuuuuuunnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnu[.     
                                       `;_1)))1}_~~~~~~~~~~~~~_[/cYXXXzcvuuvuuvuuuuuuuuuunnunnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn~      
                                            ';~]1))1[-+~~~~~~~~~~~~~?(rvXYYzcvvuuuuuuuuuuvuuuuunnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnxI      
                                                  .I])))){?~~~~~~~~~~~~~~~?jXYYXXzcuuvuuuuuuuuuuununnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnr"      
                                                       ',!-1)){[-+~~~~~~~~~~~~_{fcYYXXzzvuuuuuuuvuuuunnuunnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnr'      
                                                              ,+}1))}?++~~~~~~~~~~~+1nzXYXXXzvvvvuuuuuuuunnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnr.      
                                                                  .^;~{))1}?+~~~~~~~~~~+-}jzXYYYXXcvuuuuuuuuunnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnr'      
                                                                         ">?{)){]-+~~~~~~~~~~_1jvXYYYXzcvvuuuuuunnnnnnnnnnnnnnnnnnnnnnnnnnnnnr^      
                                                                              ."~1)))1]+~~~~~~~~~~+?/cYXYYXXzvuuuuuununnnnnnnnnnnnnnnnnnnnnnnx;      
                                                                                    "l+}))1}?_~~~~~~~~~+[(rzXYYXXzcvuuuuunnunnnnnnnnnnnnnnnnnn<      
                                                                                          ^<{1))1[_+~~~~~~~~~+}rzXXYXYXcvvuunnnnnnnnnnnnnnnnnn[.     
                                                                                               `:<[))1{[-+~~~~~~~~_])xXXYYXXzvuuuunnnnnnnnnnnnf^     
                                                                                                     ';+]1))1[-+~~~~~~~~+{fuzXYXXcvuunnunnnnnuu_     
                                                                                                          .'"<{)))1[_~~~~~~~~+_[tvYXYXXcvunnnuvn     
                                                                                                                 `I<?1)1}]-+~~~~~~~~_[(fnvzXXXc(:    
                                                                                                                        ^>})))1[_+~~~~~~~~~~~~~~<^   
                                                                                                                             ."I~[)))1}[?_~~~~~~~,   
                                                                                                                                     ^!-[}11))){+'   

)"""";

public:

// void start::printStart() {
//     cout << data << "\n";
// }

// void start::StopStart() {
//     system("cls");
//     delete data;
// }


};