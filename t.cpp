#include <iterator>
#include <regex>
#include <iostream>
#include <string>
#include <vector>

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

const char* path_to_file = "/home/box/result.out";

void exec_command(int j, std::vector<std::string> &tokens)
{
        //split command by gap
        size_t pos = 0;
        std::string token;
        std::string dlmtr = " ";
        std::vector<std::string> args_tokens;
        while ((pos = tokens[j].find(dlmtr)) != std::string::npos) {
            token = tokens[j].substr(0, pos);
            args_tokens.push_back(token);
            tokens[j].erase(0, pos + dlmtr.length());
        }
        args_tokens.push_back(tokens[j]);
        /*
        for (int i=0; i<args_tokens.size();i++){
            std::cout << args_tokens[i] << std::endl;
        }
        */
        
        int argcount = args_tokens.size();
        
        std::vector<const char*> argv;
        
        for ( const auto& s : args_tokens ) {
            argv.push_back( s.data() );
        }
       
        //argv.push_back(NULL);
        //argv.shrink_to_fit();

        /*
        for (int i=0; i<argv.size();i++){
                std::cout << argv[i] << std::endl;
            }
         */
        std::string f_comm = ""; 
        f_comm = f_comm + argv[0];
         
        std::string comms = "";
        if (argv.size() > 1) {
          for (int i=1; i<argv.size();i++){
              comms = comms + argv[i];
            }  
        }
        
        //execvp(argv[0], const_cast<char* const *>(argv.data()) );
        if(comms!="")
        {execlp(f_comm.c_str(), f_comm.c_str(), comms.c_str() , NULL);}
        else{execlp(f_comm.c_str(), f_comm.c_str(), NULL);}
}



void execute_line(std::vector<std::string> &commands, int n, int *fd)
{   
    if ( n == commands.size() -1 ){
        int fd_out = open(path_to_file, O_RDWR | O_CREAT | O_TRUNC, 0666);

        close(STDOUT_FILENO);
        dup2(fd_out, STDOUT_FILENO);
        
        exec_command(n, commands);
    } else{
        
        pipe(fd);

            if (fork()) {
            close(STDIN_FILENO);
            dup2(fd[0], STDIN_FILENO);
            close(fd[0]);
            close(fd[1]);

            int fd_2[2];

            execute_line(commands, n + 1, fd_2);

            return;
            } else {
            close(STDOUT_FILENO);
            dup2(fd[1], STDOUT_FILENO);
            close(fd[0]);
            close(fd[1]);

            exec_command(n, commands);
            }
    }
    
}



int main()
{
    //std::string str = "ls | sort"; //"who | sort | uniq -c | sort -nk1"; //"ls | sort | wc -l";
    
    std::string str;
    
    std::getline(std::cin, str);

    std::string dlmtr = " | ";
    
    size_t pos = 0;
    std::string token;
    
    std::vector<std::string> tokens;
    
    while ((pos = str.find(dlmtr)) != std::string::npos) {
        token = str.substr(0, pos);
        tokens.push_back(token);
        str.erase(0, pos + dlmtr.length());
    }
    tokens.push_back(str);
    int N_Com = tokens.size();
    /*
    for (int i=0; i<N_Com;i++){
            std::cout << tokens[i] << std::endl;
        }
    */
    /*
    char* argument_list[] = {"who"}; //{"ls", "-l",NULL};
    execvp("who",argument_list);
    */
    int fd[2];
    execute_line(tokens, 0, fd);

}
