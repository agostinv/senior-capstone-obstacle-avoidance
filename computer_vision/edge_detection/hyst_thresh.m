% Function Description: performs hysteresis thresholding on gradient image
% Inputs:
%   grad = gradient image
%       size: [dim1, dim2]
%   low_thresh = lower gradient value threshold
%   high_thresh = higher gradient value threshold
%   vert_connect_length = (half) vertical "connection" length for gradient values
%       in between 2 thresholds
%   horiz_connect_length = (half) horizontal "connection" length for gradient values
%       in between 2 thresholds
% Outputs:
%   grad = thresholded gradient image
%       size: [dim1, dim2]


function grad = hyst_thresh(grad, low_thresh, high_thresh, vert_connect_length, horiz_connect_length)

% get dimensions of gradient image:
dim = size(grad);

for i = 1+vert_connect_length : dim(1)-vert_connect_length
    for j = 1+horiz_connect_length : dim(2)-horiz_connect_length
        % if value is lower than low threshold, set to zero:
        if grad(i, j) < low_thresh
            grad(i, j) = 0;
        % if value is in between low and high threshholds:
        elseif grad(i, j) >= low_thresh && grad(i, j) < high_thresh
            % check if at least 1 neighbor has a value higher than high
            % threshold:
            connect = false;
            for p = i-vert_connect_length : i+vert_connect_length
                for q = j-horiz_connect_length : j+horiz_connect_length
                    if grad(p, q) >= high_thresh
                        connect = true;
                    end
                end
            end
            if connect==false
                grad(i, j) = 0;
            end
        % else value is higher than high threshold --> leave as is
        end
    end
end

end

