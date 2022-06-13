# INSTALL FROM BINARIES

## Since the installer does not know if Qt is installed. Currently, the installer is a CL tool

1. Copy the binaries into the final path
2. Create a folder where all the local data will be stored
3. If you have the accounts for the default plugins make sure you have your keys readily
4. On the command line run:
    1. ./install --local=[step 2 path] --bin=[step 1 path]
    2. When asked to install for the plugins. Select the necessary options.
5. If you want to run the clients on the same computer, run:
    1. ./client --default --local (FOR ALL DEFAULT CLIENTS)
    2. ./client --local --cli=[*client*.cent] (FOR A CLIENT DOWNLOADED FROM ANOTHER DEVELOPER)
    3. Also, you can install, enable and disable clients from the main UI
6. If you want to run the clients on a different computer, run:
    1. ./client --default --remote --out=[path] (FOR ALL DEFAULT CLIENTS)
    2. ./client --remote --cli=[*client*.cent] --out=[path]] (FOR A CLIENT DOWNLOADED FROM ANOTHER DEVELOPER)
    3. Also, you can install, enable and disable clients from the main UI
    4. In the remote computer download the *cril* CL tool.
    5. Run:
        1. ./cril --remote --cli=[*client*.cent] *All the default *.cent files are in the ZIP with the executable
        2. Passed the *--daemon* CL option if you want to make the client a daemon service.
        3. On linux and MacOS you must run with *sudo*
        4. On Windows you must run it with the Administrator privileges

# COMPILATION FROM SOURCE CODE

0. Install Conan
1. Install Ninja
2. Install openssl binaries
3. Install Qt 6.3 (and make sure CMake can find it otherwise there will be nasty errors)
4. Install CMake: ver > 3.21
5. Install a compiler with C++20 support. Preferred Requirements (C++11, C++14, C++17, C++20):
    1. Concepts
    2. Ranges
    3. Raw strings
    4. For ranged loops
    5. Designated initializers
    7. Non C++20 compilers will be fruitless
    8. On MacOS you can download the clang 14.0.0 compiler and redirect with -DCMAKE_C_COMPILER and -DCMAKE_CXX_COMPILER
       in CMake (at least that is how I do it). That's because Apple's clang version for C++ is not featured for
       "complete" C++20
6. Download and uncompress the source code
7. On the source code path:
    1. Create a dir named: debug
    2. Create a dir named: release
8. On the Source code path (where the CMakeLists.txt is located) run:
    1. conan install . -s build_type=Release -if conan/release -of conan2/ --build missing
    2. conan install . -s build_type=Debug -if conan/Debug -of conan2/ --build missing
9. For Debug version run:
    1. cmake -DCMAKE_BUILD_TYPE=Debug
       -DCMAKE_MAKE_PROGRAM=[ninja location]
       -DCMAKE_C_COMPILER=[path of the C compiler if you override default compiler]
       -DCMAKE_CXX_COMPILER=[path of the C compiler if you override default compiler]
       -DCMAKE_TOOLCHAIN_FILE=debug/conan_toolchain.cmake -G Ninja -S . -B build/debug
10. For Release version run:
    1. cmake -DCMAKE_BUILD_TYPE=Release
       -DCMAKE_MAKE_PROGRAM=[ninja location]
       -DCMAKE_C_COMPILER=[path of the C compiler if you override default compiler]
       -DCMAKE_CXX_COMPILER=[path of the C compiler if you override default compiler]
       -DCMAKE_TOOLCHAIN_FILE=release/conan_toolchain.cmake -G Ninja -S . -B build/release

## Before running Centaur you must set the environment, the plugin data and the client data

10. Create a path where all the local data will be
11. On the Debug or Release *bin* path:
    1. run: ./cenlocal --path=[path from step 10] (and check for error messages)
    2. **MAKE SURE THIS PATH IS TOTALLY SECURE** since sensible data will be stored in this paths
12. If you want to use the default plugins you must set the local data first. There are two ways of doing this.
    1. Step 1.
        1. Copy the default plugins.xml into [path from step 10]/Plugins/local/plugins.xml
        2. Make sure you for ExchangeRate,BinanceSPOT,BinanceFutures,ByBit,GateIO plugins have an API and secret keys
           where required
        3. For every Plugin you must run:
            1. openssl genpkey -algorithm RSA -out private_key.pem -pkeyopt rsa_keygen_bits:2048
            2. openssl rsa -pubout -in private_key.pem -out public_key.pem
        4. Encrypt all your api keys with the private key (IMPORTANT) and do not store in your computer not encrypted
           keys
        5. Write into [path from step 10]/Plugins/local/plugins.xml the encrypted data into the required fields
        6. Example:
            1. For BinanceRate (uuid="{f77ecf55-8162-5570-a9dc-3a79c6757c72}) there is the: < apiKey >< /apiKey > XML
               Element
            2. Copy the encrypted API key into that element as this:  < apiKey > *ciphered data* < /apiKey >
        7. Rename private.key to the UUID of every plugin.
        8. The private key goes into: [path from step 10]/Resources/Private/*uuid*.pem
        9. The public key goes into: [path from step 10]/Plugin/Private/*uuid*.pem
        10. Example:
            1. For ExchangeRate the uuid is: **{f77ecf55-8162-5570-a9dc-3a79c6757c72}**
            2. Run step 12.1.3.1 and 12.1.3.2
            3. Rename private.pem to {f77ecf55-8162-5570-a9dc-3a79c6757c72}.pem and copy it to [path from step 10]
               /Resources/Private/{f77ecf55-8162-5570-a9dc-3a79c6757c72}.pem
            4. Rename public.pem to {f77ecf55-8162-5570-a9dc-3a79c6757c72}.pem and copy it to [path from step 10]
               /Plugin/Private/{f77ecf55-8162-5570-a9dc-3a79c6757c72}.pem
        11. **IF YOUR EXCHANGE PLATFORM SUPPORT IP ADDRESS LIMITATION MAKE SURE TO ENABLE THIS FUNCIONALITY**. (Binance
            support this)
        12. Add the data to the Local SQLITE Database (located in [path from step 10]/Resources/Local/centaur.db).
            1. INSERT INTO plugins(name,version,manufacturer,uuid,centaur_uuid)
               VALUES ('**plugin name*', '**manufacturer*', '**uuid*', '**centaur_uuid*' );
            2. PREVENT CORRUPTING THIS FILE
            3. *name: review the name of default plugins in the source code.
            4. *manufacturer: for default plugins is: **CentaurProject**
            5. *uuid: review default plugins uuid in the source code.
            6. *centaur_uuid: minimum Centaur version. For Centaur version 0.2.0 is
               {a15c48b4-460b-4a79-a0a8-8ece90603f85}
        13. FOR BETTER COMPREHENSION OF THIS STEPS SEE PLUGIN DEVELOPMENT
        14. USE STEP 2.
    3. Step 2.
        1. On the repository there is a path named: *default*; in there are a set of files with .cent extensions
        2. run: ./ipln --file=[file] and follow the instructions
            1. Make sure you run the CL tool for every file in that path
    4. The documentation about plugins indicate how *./ipln* work as well as the structure of the .cent files as
       well as how the plugins.xml file works
    