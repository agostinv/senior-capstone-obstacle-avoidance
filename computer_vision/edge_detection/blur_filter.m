% Function Description: convolves image with a blurring filter to reduce
% noise
% Inputs:
%   pic = raw RGB image
%       size: [dim1, dim2, dim3]
%   N = dimensions of kernel matrix
%   sigma = standard deviation of Gaussian
% Outputs:
%   blur_pic = blurred image
%       size: [dim1, dim2, dim3]


function blur_pic = blur_filter(pic, N, sigma)

% get dimensions of image:
dim = size(pic);

% half size of kernel matrix:
half_N = ceil(N / 2);
% calculate Gaussian kernel matrix:
gaussian = gaussian_2D(N-2, sigma);
disp("Gaussian kernel:");
disp(gaussian);

% convolve image with Gaussian kernel to reduce noise (blur image):
blur_pic = zeros(dim(1),dim(2), dim(3));
for k = 1 : 3
    for i = half_N : dim(1)-(half_N-1)
        for j = half_N : dim(2)-(half_N-1)
            blur_pic(i, j, k) = sum(sum(pic(i-(half_N-1):i+(half_N-1), j-(half_N-1):j+(half_N-1), k) .* gaussian));
        end
    end
end
    
end




% Function Description: calculates a 2D Gaussian kernel matrix
% Inputs:
%   N = dimensions of Gaussian kernel matrix
%   sigma = standard deviation of Gaussian
% Outputs:
%   kernel = Gaussian kernel matrix
%       size: [N, N]


function kernel = gaussian_2D(N, sigma)

[x, y] = meshgrid(round(-N/2):round(N/2), round(-N/2):round(N/2));
kernel = exp(-x.^2/(2*sigma^2)-y.^2/(2*sigma^2));
kernel = kernel./sum(kernel(:));

end

