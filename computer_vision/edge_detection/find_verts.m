% Function Description: finds vertices of longer edges (length(edge) >=
%   min_edge_length)
% Inputs:
%   grad = gradient image
%       size: [dim1, dim2]
%   scan_length = vertical "scan" length (to skip zero-value pixels)
%   min_edge_length = minimum edge length to keep edge
%   max_edges = maximum number of edges to keep
%   border = width of border of image to ignore
% Outputs:
%   verts = vertices of edges
%       size: [num_edges, 2, 2]
%           verts(i, j, 1) = row number ("y"-value) of vertex j of edge i
%           verts(i, j, 2) = column number ("x"-value) of vertex j of edge
%               i


function verts = find_verts(grad, scan_length, min_edge_length, max_edges, border)

% get dimensions of gradient image:
dim = size(grad);

% vertices of edges:
% verts = zeros(max_edges, 2, 2);
% edge number:
edge_num = 1;

% single edge vertices:
edge_verts = zeros(2, 2);
% pixel location tracker:
pixel = zeros(1, 2);

for i = 1+border : dim(1)-border
    for j = 1+border : dim(2)-border
        % check for nonzero value:
        if(grad(i, j) > 0)
            % save top vertex (vertex 1):
            edge_verts(1, 1) = i;
            edge_verts(1, 2) = j;
            % starting pixel:
            pixel(1) = i;
            pixel(2) = j;
            
            % travel down (or down-left or down-right) edge:
            connect = true;
            while connect==true && pixel(2)>=2 && pixel(2)<=dim(2)-1
                % erase current pixel:
                grad(pixel(1), pixel(2)) = 0;
                
                % scan downward for first nonzero value, starting 1 row
                %   down from current pixel:
                connect = false;
                k = pixel(1)+1;
                while connect==false && k <= pixel(1)+scan_length && k<=dim(1)
                    if(grad(k, pixel(2)) > 0)
                        connect = true;
                        % move to pixel:
                        pixel(1) = k;
                    elseif(grad(k, pixel(2)-1) > 0)
                        connect = true;
                        % move to pixel:
                        pixel(1) = k;
                        pixel(2) = pixel(2)-1;
                    elseif(grad(k, pixel(2)+1) > 0)
                        connect = true;
                        % move to pixel:
                        pixel(1) = k;
                        pixel(2) = pixel(2)+1;
                    end
                    
                    % move down 1 row:
                    k = k + 1;
                end
                % if connect==false, all pixels scanned were zero => end of
                %   edge
            end
            
            % save bottom vertex (vertex 2):
            edge_verts(2, 1) = pixel(1);
            edge_verts(2, 2) = pixel(2);
            
            % add edge to verts if long enough:
            % disp(['Edge length: ', num2str(edge_length(edge_verts))]);
            if edge_length(edge_verts) >= min_edge_length
                % verts(edge_num) = edge_verts;
                for n = 1 : 2
                    for m = 1 : 2
                        verts(edge_num, n, m) = edge_verts(n, m);
                    end
                end
                edge_num = edge_num + 1;
            end
            
            % if max number of edges is reached (verts is full), return
            %   from function:
            if edge_num > max_edges
                return;
            end
        end
    end
end

end




% Function Description: calculates Euclidean distance between edge
%   vertices
% Inputs:
%   edge_verts = 2 vertices of edge (x and y-values)
%       size: [2, 2]
%           edge_verts(i, 1) = x-value of vertex i
%           edge_verts(i, 1) = y-value of vertex i
% Outputs:
%   length = Euclidean distance between vertices


function length = edge_length(edge_verts)

% calculate Euclidean distance between vertices:
length = sqrt( (edge_verts(1, 1)-edge_verts(2, 1)).^2 + (edge_verts(1, 2)-edge_verts(2, 2)).^2 ) + 1;

end

