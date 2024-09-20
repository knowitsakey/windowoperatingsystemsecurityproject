This is the code for the following assignment:

Write a program targeting the following user behavior: downloading and running an executable file, for example, in a social engineering style cyberattack. 

The program should have several mandatory features:
1. Antivirus evasion against any EDR, including an enterprise EDR at the time, which was early 2022.
2. Silent, in-memory privilege escalation via UAC bypass at any notification level, including the highest warning level, AlwaysNotify.
3. Persistence that survives a reboot, as a local admin process running at a medium integrity level
4. Operate in a single stage, in other words, do not make a callback to the C2 server until persistence is installed.
5. Optional: when people click on files they expect something to happen, for example, an image file to open. If you choose, implement this functionality.

The task was completed in the following way:
1. Antivirus evasion was accomplished completed in two ways, by using the method of calling syscalls directly, as described here: https://www.mdsec.co.uk/2020/12/bypassing-user-mode-hooks-and-direct-invocation-of-system-calls-for-red-teams/ and secondly, by encrypting the strings for the library calls to be decrypted in memory, in order to evade antivirus that works by identifying patterns of library calls associated with malware. https://vanmieghem.io/blueprint-for-evading-edr-in-2022/
3. In memory UAC bypass was accomplished by a modification of the uacme privilege escalation penetration testing tool into a dll, launching a sacrificial process, and injecting into it. https://github.com/knowitsakey/uacmeng
4. Persistence was accomplished by installing a port monitor. The port monitor masquerades as a printer driver for an older printer. https://attack.mitre.org/techniques/T1547/010/
5. Single stage operation was accomplished by iterations of process injection and loading and decrypting binary files stored in the resource section of the executable. https://ryancor.medium.com/extracting-embedded-payloads-from-malware-aaca8e9aa1a9
6. When the program is run, a jpg image is loaded from the resources section of the executable and opened using the microsoft Photos app. The size of the executable file is under 20 kilobytes without accounting for the image file.

The executable might be signed by an extended validation code signing certificate as described here: https://signmycode.com/blog/code-signing-abuse-how-to-safeguard-your-certificates#:~:text=Code%20signing%20abuse%20refers%20to,comes%20from%20a%20trusted%20source.
Or it might be served on a web page that implements a smartscreen code signing bypass, a vulnerability which is hopefully fixed for the future: https://msrc.microsoft.com/update-guide/vulnerability/CVE-2023-36025

The key learning points in this project were:
1. Built an intimate understanding of Windows operating system security.
2. Understand how to detect and trace malware infections on microsoft windows based on a knowledge of idiosyncratic malware coding techniques.

Under the security model of the microsoft windows operating system, antivirus products can always be bypassed. Once an adversary controls a process, there are numerous ways to evade, elevate, and persist. A human expert still must always be in the loop in situations involving malware.
