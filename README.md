***ESTIMAREA ADANCIMII IMAGINILOR FOLOSIND TEHNICI STEREO SI RECTIFICARE A IMAGINILOR***



**ANALIZA LITERATURII DE SPECIALITATE**


| Nr. | Autor/ An | Titlul articolului/proiectului | Aplicatie/Domeniu | Tehnologii utilizate | Metodologie/Abordare | Rezultate | Limitari |
|---|---|---|---|---|---|---|---|
|1|Qiaochu ZHAO, Ittetsu TANIGUCHI, Takao ONOYE / 2020 |A template-free object motion estimation method for industrial vision system in aligning machine|Industrial aligning machine|Intel Xeon CPU|Magic Line Algorithm, Edge Extraction Algorithm|
|2|Sumit K. Chatterjee, Sravan K. Vittapu/2019|An Efficient Motion Estimation Algorithm for Mobile Video Applications|Video compression|Block matching algorithm| |Average PSNR: 26.90%|
|3|Wenjuan Lu , Huiting Ma , and Daxing Zeng /2024|Performance of a Novel Muscle Synergy Approach for Continuous Motion Estimation on Untrained Motion| Human- robot interaction|Ninapro Database2, 22-sensor CyberGlove II, Bluetooth tunnel serial port,  Trigno Wireless electrodes|NARX neural network| Highest prediction accuracy of 96.3%|Inadequate number of experimental subjects|
|4|Chang Sun, Wenpeng Ren, Jia Su*, Yong Guan /2019|Fast Motion Estimation Method for Self-driving Video|Self-driving cars, video processing, video compression|High Efficiency Video Coding (HEVC), Motion estimation algorithms|DFMD (Fast Prediction Mode Selection), DFMVP (Fast Motion Vector)|Reduce the encoding time by 30%|Small quality loss|
|5|Qinke Yang, Chenming Li, Yuan Li /2022|Human Motion Monitoring and Estimation based on Optical Measurement|Medical, records and analyzes the changes of the main joint angles of the human body|The optical motion capture system Optitrack, Neural Network|Design a neural network model to estimate the amount of motion|The exercise volume score was 73.44%|

**SCHEMA BLOC A PROIECTULUI**

![schema bloc](https://github.com/user-attachments/assets/af99111e-cd91-4e2b-bdd0-862181e0c2b9)
**EXPLICATII**
1. Daca imaginile stereo sunt color,conversia acestora in alb-negru poate reduce complexitatea, utilizarea unor filtre pentru a reduce zgomotul ce poate afecta calitatea detectarii miscarii si ajustarea valorilor de intensitate pentru a avea o imagine mai uniforma si a elimina efectele cauzate de variatii in iluminare
2. Extragere perechi de cadre stereo si alinierea punctelor corespondente pe linnii orizontale, calculul disparitatii si adancimea pentru fiecare punct.Identificarea punctelor corespondente între cele două imagini. Pentru fiecare pixel din imaginea stângă se caută pixelul corespunzător din imaginea dreaptă, rezultând harta de disparitate, care reprezintă diferențele de poziție ale punctelor între cele două imagini.
3. Calcul optical flow pentru a estima miscarea intre cadre consecutive
4. Combinarea datelor obtinute prin disparitate si optical low pentru a determina miscarea 3D a obiectelor in scena
5. Aplicarea filtrelor de netezire pentru a elimina variatiile de miscare nejustificate si zgomotul de calcul
6. Segmentarea obiectelor care se misca in scena
7. Reprezentare vizuala a miscarii prin sageti vectoriale sau harti de culori pentru a verifica acuratetea algoritmului
