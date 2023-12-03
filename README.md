## RVC log extractror CLI

This programm scans train.log file which is created due training the RVC model, collect loss_disc loss_gen loss_fm loss_mel loss_kl and stores this values in xlsx table

**Only x64 system!**

**License: no license**

**Uses OpenXLSX library**

**MSVC 2019**

This project requirements c++17 compatible compiler

Usage: rvc_logs_extractrion.exe train.log

Programm will generates .xlsx(excel) table that contains 5 columns like:

|loss_disc|loss_gen|loss_fm|loss_mel|loss_kl|
|-|-|-|-|-|
|4.390|4.636|14.265|28.126|6.896|
|4.222|3.181|7.311|18.642|2.161|
|...|...|...|...|...|
|3.480|3.694|13.850|18.037|0.621|

just insert plot and it will automatically generates graph lines

README.md was created in [LiteMD](https://github.com/anrej0705/LiteMD/releases) program
