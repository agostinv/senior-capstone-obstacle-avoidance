% Function Description: forms an image that only contains estimated
%   straight-line edges
% Inputs:
%   verts = vertices of edges
%       size: [num_edges, 2, 2]
%           verts(i, j, 1) = row number ("y"-value) of vertex j of edge i
%           verts(i, j, 2) = column number ("x"-value) of vertex j of edge
%               i
%   dim = dimensions of gradient image
%   pixel_value = intensity value to be used of edges
% Outputs:
%   edge_display = image displaying estimated straight-line edges
%       size: [dim(1), dim(2)]


function edge_display = display_edges(verts, dim, pixel_value, edge_width)

% image to display edges:
edge_display = zeros(dim(1), dim(2));

dim_vert = size(verts);
for edge = 1 : dim_vert(1)
    % vertex 1 coordinates:
    x1 = verts(edge, 1, 2);
    y1 = verts(edge, 1, 1);
    % vertex 2 coordinates:
    x2 = verts(edge, 2, 2);
    y2 = verts(edge, 2, 1);
    
    % determine which vertex is more left:
    % if vertex 1 is more left:
    if x1 <= x2
        x_left = x1;
        y_left = y1;
        x_right = x2;
        y_right = y2;
    % else vertex 2 is more left:
    else
        x_left = x2;
        y_left = y2;
        x_right = x1;
        y_right = y1;
    end
    
    % approximate slope:
    y_slope = round( 1 * ( (y_right-y_left) / (x_right-x_left) ) );
    x_slope = 1;
    if(abs(y_slope) > dim(1) / 2)
        % fill in pixels in straight line down:
        edge_display(min(y_left, y_right):max(y_left, y_right), x_left-edge_width:x_right+edge_width) = pixel_value;
        continue;
    end
    
    % start at left vertex:
    j = x_left;
    i = y_left;
    % fill in straight line from left vertex to right vertex:
    while i >= min(y_left, y_right) && i <= max(y_left, y_right) && i+y_slope >= 1 && i+y_slope <= dim(1) && j-edge_width >= 1 && j+edge_width <= dim(2)
        % fill in pixels in straight line down:
        if y_slope >= 0
            edge_display(i:i+y_slope, j-edge_width:j+edge_width) = pixel_value;
        else
            edge_display(i+y_slope:i, j-edge_width:j+edge_width) = pixel_value;
        end
        % move to next pixel:
        i = i + y_slope;
        j = j + x_slope;
    end
end

end