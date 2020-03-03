close;
clear;
clc;



% read in raw image:
raw_pic = imread("watermelon_3.jpg");
% display image info:
disp("Raw JPEG image info:");
whos raw_pic;
% get dimensions of image:
dim = size(raw_pic);

% display original image:
figure(1);
imshow(raw_pic);

% convert data type of raw_pic to double:
pic = double(raw_pic);

% % array for testing:
% pic = zeros(7, 7, 3);
% dim = size(pic);
% for k = 1 : 3
%     for i = 1 : dim(1)
%         pic(i, 3, k) = k;
%         pic(i, 5, k) = 2*k+1;
%     end
% end



% --------------------------------------------------
% 1) GAUSSIAN BLUR FILTER:

% dimensions and standard deviation of Gaussian kernel:
kernel_size = 3;
sigma = 1;

% blur image with Gaussian kernel:
blur_pic = blur_filter(pic, kernel_size, sigma);

% convert data type of filtered image to uint8:
blur_pic_disp = uint8(blur_pic);
% display filtered image:
figure(2);
imshow(blur_pic_disp);



% --------------------------------------------------
% 2) SOBEL GRADIENT ESTIMATION:

% estimate gradients:
grad = estimate_grad(blur_pic);

% convert data type of gradient to uint8:
grad_disp = uint8(grad);
% display gradient image:
figure(3);
imshow(grad_disp);



% --------------------------------------------------
% 3) HYSTERESIS THRESHOLDING:

% lower gradient threshold:
low_thresh = 40;
% higher gradient threshold:
high_thresh = 60;
% vertical connection (half) length:
vert_connect_length = 10;
% horizontal connection (half) length:
horiz_connect_length = 2;

% perform hysterisis thresholding:
grad = hyst_thresh(grad, low_thresh, high_thresh, vert_connect_length, horiz_connect_length);

% convert data type of gradient to uint8:
grad_disp = uint8(grad);
% display thresholded gradient image:
figure(4);
imshow(grad_disp);



% --------------------------------------------------
% 4) NON-MAXIMAL SUPPRESSION:

% (half) horizontal "suppression" length:
suppress_length = 20;

% perform non-maximal suppression:
grad = non_max_suppress(grad, suppress_length);

% convert data type of gradient to uint8:
grad_disp = uint8(grad);
% display non-maximally suppressed gradient image:
figure(5);
imshow(grad_disp);



% --------------------------------------------------
% 5) LONG-EDGE SELECTION AND EDGE VERTEX DETERMINATION:

% dimensions of gradient image:
dim_grad = size(grad);

% vertical connection length:
scan_length = 10;
min_edge_length = 50;
max_edges = 10;
disp(newline);
disp(['Max number of edges: ', num2str(max_edges)]);
border = 20;

% find edge vertices:
verts = find_verts(grad, scan_length, min_edge_length, max_edges, border);

% get size of verts:
dim_verts = size(verts);
disp(['Number of edges: ', num2str(dim_verts(1))]);
disp(['Size of edge vertex array: ', num2str(dim_verts)]);
disp(newline);
% display edge vertices coordinates:
for edge = 1 : dim_verts(1)
    disp(['Edge ', num2str(edge), ':']);
    for i = 1 : 2
        disp(['vertex ', num2str(i), ' coordinates: (', num2str( verts(edge, i, 2) ), ' ,', num2str( verts(edge, i, 1) ), ')']);
    end
    disp(newline);
end

% display straight-line edges:
% pixel brightness:
bright = 255;
% edge width:
edge_width = 1;
edge_display = display_edges(verts, dim_grad, bright, edge_width);

% convert data type of edge_display to uint8:
edge_display = uint8(edge_display);
% display edges:
figure(6);
imshow(edge_display);



