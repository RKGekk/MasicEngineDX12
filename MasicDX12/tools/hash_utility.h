#pragma once

#include <d3d12.h>

#include <cstddef>
#include <functional>

namespace std {
    template <typename T>
    inline void hash_combine(std::size_t& seed, const T& v) {
        std::hash<T> hasher;
        seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }

    template<>
    struct hash<D3D12_SHADER_RESOURCE_VIEW_DESC> {
        std::size_t operator()(const D3D12_SHADER_RESOURCE_VIEW_DESC& srv_desc) const noexcept {
            std::size_t seed = 0;

            hash_combine(seed, srv_desc.Format);
            hash_combine(seed, srv_desc.ViewDimension);
            hash_combine(seed, srv_desc.Shader4ComponentMapping);

            switch (srv_desc.ViewDimension) {
                case D3D12_SRV_DIMENSION_BUFFER:
                    hash_combine(seed, srv_desc.Buffer.FirstElement);
                    hash_combine(seed, srv_desc.Buffer.NumElements);
                    hash_combine(seed, srv_desc.Buffer.StructureByteStride);
                    hash_combine(seed, srv_desc.Buffer.Flags);
                    break;
                case D3D12_SRV_DIMENSION_TEXTURE1D:
                    hash_combine(seed, srv_desc.Texture1D.MostDetailedMip);
                    hash_combine(seed, srv_desc.Texture1D.MipLevels);
                    hash_combine(seed, srv_desc.Texture1D.ResourceMinLODClamp);
                    break;
                case D3D12_SRV_DIMENSION_TEXTURE1DARRAY:
                    hash_combine(seed, srv_desc.Texture1DArray.MostDetailedMip);
                    hash_combine(seed, srv_desc.Texture1DArray.MipLevels);
                    hash_combine(seed, srv_desc.Texture1DArray.FirstArraySlice);
                    hash_combine(seed, srv_desc.Texture1DArray.ArraySize);
                    hash_combine(seed, srv_desc.Texture1DArray.ResourceMinLODClamp);
                    break;
                case D3D12_SRV_DIMENSION_TEXTURE2D:
                    hash_combine(seed, srv_desc.Texture2D.MostDetailedMip);
                    hash_combine(seed, srv_desc.Texture2D.MipLevels);
                    hash_combine(seed, srv_desc.Texture2D.PlaneSlice);
                    hash_combine(seed, srv_desc.Texture2D.ResourceMinLODClamp);
                    break;
                case D3D12_SRV_DIMENSION_TEXTURE2DARRAY:
                    hash_combine(seed, srv_desc.Texture2DArray.MostDetailedMip);
                    hash_combine(seed, srv_desc.Texture2DArray.MipLevels);
                    hash_combine(seed, srv_desc.Texture2DArray.FirstArraySlice);
                    hash_combine(seed, srv_desc.Texture2DArray.ArraySize);
                    hash_combine(seed, srv_desc.Texture2DArray.PlaneSlice);
                    hash_combine(seed, srv_desc.Texture2DArray.ResourceMinLODClamp);
                    break;
                case D3D12_SRV_DIMENSION_TEXTURE2DMS:
                    break;
                case D3D12_SRV_DIMENSION_TEXTURE2DMSARRAY:
                    hash_combine(seed, srv_desc.Texture2DMSArray.FirstArraySlice);
                    hash_combine(seed, srv_desc.Texture2DMSArray.ArraySize);
                    break;
                case D3D12_SRV_DIMENSION_TEXTURE3D:
                    hash_combine(seed, srv_desc.Texture3D.MostDetailedMip);
                    hash_combine(seed, srv_desc.Texture3D.MipLevels);
                    hash_combine(seed, srv_desc.Texture3D.ResourceMinLODClamp);
                    break;
                case D3D12_SRV_DIMENSION_TEXTURECUBE:
                    hash_combine(seed, srv_desc.TextureCube.MostDetailedMip);
                    hash_combine(seed, srv_desc.TextureCube.MipLevels);
                    hash_combine(seed, srv_desc.TextureCube.ResourceMinLODClamp);
                    break;
                case D3D12_SRV_DIMENSION_TEXTURECUBEARRAY:
                    hash_combine(seed, srv_desc.TextureCubeArray.MostDetailedMip);
                    hash_combine(seed, srv_desc.TextureCubeArray.MipLevels);
                    hash_combine(seed, srv_desc.TextureCubeArray.First2DArrayFace);
                    hash_combine(seed, srv_desc.TextureCubeArray.NumCubes);
                    hash_combine(seed, srv_desc.TextureCubeArray.ResourceMinLODClamp);
                    break;
            }

            return seed;
        }
    };

    template<>
    struct hash<D3D12_UNORDERED_ACCESS_VIEW_DESC> {
        std::size_t operator()(const D3D12_UNORDERED_ACCESS_VIEW_DESC& uav_desc) const noexcept {
            std::size_t seed = 0;

            hash_combine(seed, uav_desc.Format);
            hash_combine(seed, uav_desc.ViewDimension);

            switch (uav_desc.ViewDimension) {
                case D3D12_UAV_DIMENSION_BUFFER:
                    hash_combine(seed, uav_desc.Buffer.FirstElement);
                    hash_combine(seed, uav_desc.Buffer.NumElements);
                    hash_combine(seed, uav_desc.Buffer.StructureByteStride);
                    hash_combine(seed, uav_desc.Buffer.CounterOffsetInBytes);
                    hash_combine(seed, uav_desc.Buffer.Flags);
                    break;
                case D3D12_UAV_DIMENSION_TEXTURE1D:
                    hash_combine(seed, uav_desc.Texture1D.MipSlice);
                    break;
                case D3D12_UAV_DIMENSION_TEXTURE1DARRAY:
                    hash_combine(seed, uav_desc.Texture1DArray.MipSlice);
                    hash_combine(seed, uav_desc.Texture1DArray.FirstArraySlice);
                    hash_combine(seed, uav_desc.Texture1DArray.ArraySize);
                    break;
                case D3D12_UAV_DIMENSION_TEXTURE2D:
                    hash_combine(seed, uav_desc.Texture2D.MipSlice);
                    hash_combine(seed, uav_desc.Texture2D.PlaneSlice);
                    break;
                case D3D12_UAV_DIMENSION_TEXTURE2DARRAY:
                    hash_combine(seed, uav_desc.Texture2DArray.MipSlice);
                    hash_combine(seed, uav_desc.Texture2DArray.FirstArraySlice);
                    hash_combine(seed, uav_desc.Texture2DArray.ArraySize);
                    hash_combine(seed, uav_desc.Texture2DArray.PlaneSlice);
                    break;
                case D3D12_UAV_DIMENSION_TEXTURE3D:
                    hash_combine(seed, uav_desc.Texture3D.MipSlice);
                    hash_combine(seed, uav_desc.Texture3D.FirstWSlice);
                    hash_combine(seed, uav_desc.Texture3D.WSize);
                    break;
            }

            return seed;
        }
    };
}