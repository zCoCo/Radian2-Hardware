BW = imread('circles.png');
  %simple:
  skel = voronoiSkel(BW);
  % setting trim and fast:
  [skel v e] = voronoiSkel(BW,'trim',2,'fast',1.23);
  % boundary input:
  b = bwboundaries(BW); b = b{1};
  [v e] = voronoiSkel(b,'boundary','trim',2,'fast',1.23);