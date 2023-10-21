#include <mmio.h>
#include <paddr.h>
#include <cpu.h>

#define IO_SPACE_SIZE (2 * 1024 * 1024)

struct cmpMap
{
    bool operator()(const std::pair<paddr_t, paddr_t> & l, const std::pair<paddr_t, paddr_t>& r) const
    { return l.first < r.first;}
};

std::map<std::pair<paddr_t, paddr_t>, io_map_t, cmpMap> io_space;

void register_io_map(const char* name, paddr_t addr, paddr_t len, void* base, io_handler_t handler){
    paddr_t low = addr;
    paddr_t high = addr + len - 1;
    assert(low < high);
    assert(high - low < IO_SPACE_SIZE);
    // check whether the addr is in pmem: 0x1c000000 ~ 0x24000000
    assert(low > CONFIG_PMEM_BASE + CONFIG_PMEM_SIZE || high < CONFIG_PMEM_BASE);
    // check if overlap with other io_map
    auto iter = std::find_if(io_space.begin(), io_space.end(), [low, high](const std::pair<std::pair<paddr_t, paddr_t>, io_map_t>& p){
        return !(p.first.first > high || p.first.second < low);
    });
    assert(iter == io_space.end());
    // assert(handler != NULL);
    io_space[std::make_pair(low, high)] = {name, base, handler};
    Log("Register io map: %s at 0x%08x ~ 0x%08x", name, low, high);
}

word_t mmio_read(paddr_t addr, uint32_t len){
    uint32_t low = addr;
    uint32_t high = addr + len - 1;
    auto iter = std::find_if(io_space.begin(), io_space.end(), [low, high](const std::pair<std::pair<paddr_t, paddr_t>, io_map_t>& p){
        return low >= p.first.first && high <= p.first.second;
    });
    if(iter == io_space.end()){
        std::cout << "Read: Physical address " << std::hex << addr << " is outside of io space!" << std::endl;
        cpu.state = SIM_ABORT;
        return 0;
    }
    paddr_t offset = addr - iter->first.first;
    if(iter->second.handler != NULL) iter->second.handler(iter->second.base, offset, len, false);
    return host_read((uint8_t*)iter->second.base + offset, len);
}

void mmio_write(paddr_t addr, word_t data, uint32_t len){
    uint32_t low = addr;
    uint32_t high = addr + len - 1;
    auto iter = std::find_if(io_space.begin(), io_space.end(), [low, high](const std::pair<std::pair<paddr_t, paddr_t>, io_map_t>& p){
        return low >= p.first.first && high <= p.first.second;
    });
    if(iter == io_space.end()){
        std::cout << "Write: Physical address " << std::hex << addr << " is outside of io space!" << std::endl;
        cpu.state = SIM_ABORT;
        return;
    }
    paddr_t offset = addr - iter->first.first;
    host_write((uint8_t*)iter->second.base + offset, data, len);
    if(iter->second.handler != NULL) iter->second.handler(iter->second.base, offset, len, true);
}

