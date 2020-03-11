% Function Description: performs non-maximal suppression on gradient image
% Inputs:
%   grad = gradient image
%       size: [dim1, dim2]
%   suppress_length = (half) horizontal "suppression" length
% Outputs:
%   grad = non-maximally suppressed gradient image
%       size: [dim1, dim2]


function grad = non_max_suppress(grad, suppress_length)

% get dimensions of gradient image:
dim = size(grad);

for i = 1 : dim(1)
    for j = 1+suppress_length : dim(2)-suppress_length
        % if gradient is not max in suppression range, set to zero:
        if grad(i, j) ~= max(grad(i, j-suppress_length:j+suppress_length))
            grad(i, j) = 0;
%         else
%             grad(i, j) = 255;
        end
    end
end

end

