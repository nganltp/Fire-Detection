# Đồ án môn mã nguồn mở

* Giáo viên hướng dẫn: thầy **Huỳnh Tuấn Anh**
* Sinh viên thực hiện: **Lê Thị Phương Ngân** - MSSV: **16520792**.

Trường Đại học công nghệ thông tin, ĐHQG-TPHCM.

## Hệ Thống Nhận Diện Khói Lửa Bằng Camera

* Công nghệ: OpenCV 3.4, SVM (Support Vector Machine).
* Ngôn ngữ: C++, Python.
* Video Demo: [tại đây](https://www.youtube.com/playlist?list=PLzw57v8RSaORY7mJrx5TZPwho8NWilDdL)
* Báo cáo: [Fire Detection Report](https://docs.google.com/document/d/1qZi1DAUL6XPc7dVrN5u6yIfvwSeJ38vyf22Y9CDTba8/edit?usp=sharing)


## Hướng dẫn cài đặt môi trường
* Window, Visual Studio 2019.
* OpenCV 3.4.1.

Mở Windows PowerShell: chạy lệnh:

```bash
cd Fire-Detection\Motion2Cpp\x64\Release\
```

```bash
./Motion2Cpp.exe [Video Path]
```

ví dụ:
```bash
./Motion2Cpp.exe \Fire-Detection\videoTest\test.mp4
```


## Phương pháp thực hiện
I. Image Processing.
  - Background Subtraction.

 - Fire Color Filtering.

 - Mophological Processing.

 - Connected Component Labeling. 

II. Foreground region analysis.

III. Fire Dynamic Behavior Analysis

IV. Fire Flow Energy Analysis

## Tham khảo
[1] G. Healey, D. Slater, T. Lin, B. Drda, and D. Goedeke.
“A system for real-time fire detection,” IEEE
International Conference on Computer Vision and
Pattern Recognition, pp. 605 – 606, January 1993


[2] C. B. Liu and N. Ahuja ”Vision Based Fire Detection,
IEEE International Conference on Pattern
Recognition,Vol. 4, pp. 134 – 137, August 2004.


[3] H.D. Duong, D.D. Nguyen, L.T. Ngo, and D.T. Tinh,
“On Approach to Vision Based Fire Detection Based on
Type-2 Fuzzy Clustering,” IEEE International
Conference on Soft Computing and Pattern Recognition,
pp. 51-56, October 2011.


