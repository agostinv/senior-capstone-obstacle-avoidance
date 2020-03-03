% Function Description: estimates gradients of image using Sobel operator
% Inputs:
%   pic = RGB image
%       size: [dim1, dim2, dim3]
% Outputs:
%   grad = 2D matrix of estimated gradients
%       size: [dim1, dim2]


function grad = estimate_grad(pic)

% get dimensions of image:
dim = size(pic);

total_weight = 4;
% horizontal gradient Sobel kernel (for vertical edges):
sobel_horiz = zeros(3);
sobel_horiz(1,1) = -1;
sobel_horiz(2,1) = -2;
sobel_horiz(3,1) = -1;
sobel_horiz(1,3) = 1;
sobel_horiz(2,3) = 2;
sobel_horiz(3,3) = 1;
sobel_horiz = sobel_horiz / total_weight;
disp("Horizontal Sobel operator:");
disp(sobel_horiz);
% vertical gradient Sobel kernel (for horizontal edges):
sobel_vert = sobel_horiz';
disp("Vertical Sobel operator:");
disp(sobel_vert);

% convolve image with Sobel kernels to estimate gradients:
grad_horiz = zeros(dim(1),dim(2), dim(3));
grad_vert = zeros(dim(1),dim(2), dim(3));
grad_norm = zeros(dim(1),dim(2), dim(3));
grad = zeros(dim(1),dim(2));
for i = 2 : dim(1)-1
    for j = 2 : dim(2)-1
        for k = 1 : 3
            % calculate horizontal gradient:
            grad_horiz(i, j, k) = sum(sum(pic(i-1:i+1, j-1:j+1, k) .* sobel_horiz));
            % calculate vertical gradient:
            grad_vert(i, j, k) = sum(sum(pic(i-1:i+1, j-1:j+1, k) .* sobel_vert));
            % calculate norm of horizontal and vertical gradients:
            grad_norm(i, j, k) = sqrt(grad_horiz(i, j, k).^2 + grad_vert(i, j, k).^2);
        end
        % take maximum gradient over all colors:
        grad(i, j) = max(grad_norm(i, j, :));
    end
end

% print max gradient value:
disp("Max gradient value:");
disp(max(max(grad)));
    
end

