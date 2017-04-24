1. 學號：b03901016
2. 姓名：陳昊
3. 使用之程式語言：< C++ >
4. 使用之編譯器：< GNU g++ (-std=c++11) >
5. 檔案壓縮方式: <tar zcvf Floorplanner.tgz Floorplanner>
6. 各檔案說明：
	 Floorplanner/src/: source code repo
	 Floorplanner/lib/: include library repo
	 Floorplanner/input/: input cases
	 Floorplanner/output/: outputs
	 Floorplanner/Floorplanner.exe: executable binary
	 Floorplanner/report.doc ： report

7. 編譯方式說明：        	
   主程式：Floorplanner/Floorplanner
	 請在主程式的目錄下，鍵入make指令，即可完成編譯，
	 ( Makefile原先下的 Optimize 指令為 -O3)
	 在主程式的目錄下會產生一個名為 Floorplanner 的執行檔
	 如果要重新編譯，請先執行 make clean 再執行一次 make

           . . . . . .
8. 執行、使用方式說明：
   主程式：
   編譯完成後，在檔案目錄下會產生一個 fm 的執行檔
   執行檔的命令格式為：
   ./Floorplanner.exe <alpha> <input block name> <input net name> <output file name>

           . . . . . .         
9. 執行結果說明（說明執行結果的觀看方法，及解釋各項數據等）：
   主程式：
	 主程式執行完成之後會產生一個內容與 console 上顯示相同的檔案
	 (輸入的output file name Ex. output_1.dat)
	
       

