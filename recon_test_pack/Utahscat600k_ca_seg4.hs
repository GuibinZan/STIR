!INTERFILE  :=
name of data file := Utahscat600k_ca_seg4.s
originating system := PRT-1
!GENERAL DATA :=
patient orientation := head_in
patient rotation :=  supine
!GENERAL IMAGE DATA :=
!type of data := PET
imagedata byte order := LITTLEENDIAN
!PET STUDY (General) :=
!PET data type := Emission
applied corrections:={arc correction}
!number format := float
!number of bytes per pixel := 4
number of dimensions := 4
matrix axis label [4] := segment
!matrix size [4] := 9
matrix axis label [2] := view
!matrix size [2] := 96
matrix axis label [3] := axial coordinate
!matrix size [3] := { 12,13,14,15,16,15,14,13,12}
matrix axis label [1] := tangential coordinate
!matrix size [1] := 128
minimum ring difference per segment := { -4,-3,-2,-1,0,1,2,3,4}
maximum ring difference per segment := { -4,-3,-2,-1,0,1,2,3,4}
number of rings := 16
number of detectors per ring := 384
distance between rings (cm) := 0.675
; This offset is constructed such that it compensates for the offset implied
; in the view-mashing. This means that the first view will be "vertical"
; as it was in older versions of STIR
view offset (degrees) := -0.46875
number of time frames := 1
image duration (sec) [1]:= 60
image relative start time (sec)[1] := 100
!END OF INTERFILE :=
