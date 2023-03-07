#include "includes.h"

void commands()
{
    string cmd;
    while (1)
    {
        cin >> cmd;

        if (cmd == "test")
        {
            printf("test succeed\n");
        }
        else if (cmd == "help")
        {
            cout << "1. test - does nothing" << endl
                << "2. help - list of commands" << endl
                << "3. ban - blacklist hwid" << endl
                << "4. add - make license" << endl;
        }
        else if (cmd == "ban")
        {
            string ban;
            printf("enter hwid to ban: ");
            cin >> ban;

            file.open("bans", ios_base::app);
            file << ban << endl;
            file.close();

            printf("successfully banned %s\n", ban.c_str());
        }
        else if (cmd == "add")
        {
            //license parameters
            string length, license;
            int count, choice, custom;
            printf("how many licenses: ");
            cin >> count;
            printf("length (1=1d, 2=7d, 3=14d, 4=30d, 5=custom in hours): ");
            cin >> choice;
            if (choice == 1)
                length = "1__d";
            else if (choice == 2)
                length = "7__d";
            else if (choice == 3)
                length = "14__d";
            else if (choice == 4)
                length = "30__d";
            else if (choice == 5)
            {
                printf("time in hours (e.g. 72): ");
                cin >> custom;
                length = to_string(custom) + "__h";
            }
            else
                printf("invalid choice\n");

            for (int i = 0; i < count; i++)
            {
                //gen license
                license = gen_random_ms(4, 983642) + "-" + gen_random_ms(4, 1024) + "-" + gen_random_ms(4, 7890) + "-" + gen_random_ms(4, 234768596);

                //check if license already exists
            trygain:
                ifstream l("licenses/" + license);
                string expire;
                if (l)
                {
                    license = gen_random_ms(4, 92) + "-" + gen_random_ms(4, 14) + "-" + gen_random_ms(4, 92) + "-" + gen_random_ms(4, 348);
                    goto trygain; //need to set new value to the ifstream varialbe
                }
                file.open("licenses/" + license, ios_base::app);
                file << "notused " << length << endl;
                file.close();
            }

            printf("successfully generated %i licenses\n", count);
        }
        else if (cmd == "reset")
        {
            string license;
            printf("enter license which you want to reset hwid at: ");
            cin >> license;

            ifstream l("licenses/" + license);
            if (l)
            {
                string oldhwid, secondData;
                l >> oldhwid;
                if (oldhwid == "notused")
                    printf("the license has no hwid bound\n");
                else
                {
                    l >> secondData;

                    file.open("licenses/" + license);
                    file << "hwidreset " << secondData << endl;
                    file.close();

                    printf("successfully resetted hwid for %s\n", license.c_str());
                }
            }
            else
            {
                printf("license not found\n");
            }
        }
        else
        {
            printf("unknown command\n");
        }
    }
}