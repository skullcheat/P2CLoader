#include "includes.h"

void on_client_connect(LPVOID Ar)
{
    //getting data from the structure
    ARGS* Args = (ARGS*)Ar;
    SOCKET client = *Args->soc;
    string clientAddr = *Args->str;

    connections++; //just to know how many people are conencted at once
    printf("currently connected users: %i\n", connections);

    printf("client connected from %s\n", clientAddr.c_str());

    char buffer[2048]; //main receiving buffer

    //initialization data (initdata, version, pid, hwid, license)
    if (recv(client, buffer, sizeof(buffer), 0) > 0) //doesnt crash server if user does not input anything
    {
        printf("%s says raw: %s\n", clientAddr.c_str(), buffer);

        //decrypt received data and initialize decryption key
        int pid;
        string delimiter = "__", pidRaw, buff0Enc, hwid, version1, license, initcheck, s(buffer);
        size_t pos = 0;

        //split data needed for decryption key and other
        pos = s.find(delimiter);
        buff0Enc = s.substr(0, pos);
        s.erase(0, pos + delimiter.length());
        pidRaw = s;
        pid = stoi(pidRaw);
        pid = (pid + 796) / 3;

        //make decryption key
        string akey = gen_random(32, pid);
        string aiv = gen_random(16, pid);
        string initData = security::decrypt(buff0Enc, akey, aiv);

        //check if we have correct decryption key and if not, try make a new one in case request has been delayed (currently 3sec delay allowed)
        int i = 1;
        while (initData.find("initcheck") == string::npos && i <= 3) //decrypted data not contain "initcheck" -> we have bad decryption key
        {
            akey = gen_random_with_delay(32, pid, i);
            aiv = gen_random_with_delay(16, pid, i);
            initData = security::decrypt(buff0Enc, akey, aiv);
            ++i;
        }
        if (initData.find("initcheck") != string::npos) {}//we have successfully found the key
        else
            sendEnc(client, "badinitcheck", akey, aiv);

        //split other received data (initcheck, version, license, hwid)
        pos = 0;

        pos = initData.find(delimiter);
        initcheck = initData.substr(0, pos);
        initData.erase(0, pos + delimiter.length());

        pos = initData.find(delimiter);
        hwid = initData.substr(0, pos);
        initData.erase(0, pos + delimiter.length());

        pos = initData.find(delimiter);
        version1 = initData.substr(0, pos);
        initData.erase(0, pos + delimiter.length());

        license = initData;

        //count length of client ip address (just for cosmetic stuff)
        string blank;
        for (int blankI = 0; blankI < clientAddr.length(); blankI++)
            blank += " ";

        printf("%s says decrypted:\n", clientAddr.c_str());
        printf("%s initcheck: %s\n", blank.c_str(), initcheck.c_str());
        printf("%s hwid:      %s\n", blank.c_str(), hwid.c_str());
        printf("%s version:   %s\n", blank.c_str(), version1.c_str());
        printf("%s license:   %s\n", blank.c_str(), license.c_str());

        //version check
        if (version != version1)
            sendEnc(client, "invalidver", akey, aiv);
        else
            sendEnc(client, "goodver", akey, aiv);

        //license and ban check
        //here I Would suggest to make a website and use an actual database as file management sucks
        //only reason I use file system here is that not everyone has a website, well I may add it later anyways
        ifstream b("bans");
        string bannedHwid;
        while (getline(b, bannedHwid))
        {
            if (hwid == bannedHwid)
                sendEnc(client, "hwidban", akey, aiv);
        }

        ifstream l("licenses/" + license);
        string usedornot;
        if (l)
        {
            l >> usedornot;
            if (usedornot == "notused") //if license not used yet (made so people can use license later after purchase without wasting license time)
            {
                l >> usedornot; //old value is skipped, so it reads length of the license (30__d etc)

                //check type of the length (days or hours) and split actual length and the type
                string length, hourorday;
                int len;

                pos = 0;
                pos = usedornot.find(delimiter);
                length = usedornot.substr(0, pos); //length
                usedornot.erase(0, pos + delimiter.length());

                hourorday = usedornot; //day or hour

                len = stoi(length);

                if (hourorday == "d")
                    len = len * 24 * 3600;
                else if (hourorday == "h")
                    len = len * 3600;
                else
                    printf("invalid length type\n");

                file.open("licenses/" + license);
                file << hwid << " " << time(NULL) + len;
                file.close();

                sendEnc(client, "good", akey, aiv);
            }
            else
            {
                int expire;
                l >> expire;
                if (usedornot == hwid)
                {
                    if (expire > time(NULL))
                        sendEnc(client, "good", akey, aiv);
                    else
                        sendEnc(client, "expired", akey, aiv);

                }
                else if (usedornot == "hwidreset")
                {
                    file.open("licenses/" + license);
                    file << hwid << " " << expire << endl;
                    file.close();

                    if (expire > time(NULL))
                        sendEnc(client, "good", akey, aiv);
                    else
                        sendEnc(client, "expired", akey, aiv);
                }
                else
                    sendEnc(client, "badhwid", akey, aiv);
            }
        }
        else
            sendEnc(client, "invalid", akey, aiv);
    }

    closesocket(client);
    printf("%s disconnected\n", clientAddr.c_str());
    connections--;
}