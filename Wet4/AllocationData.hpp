class AllocationData {
public:
   AllocationData(uint size) {
      is_free = false;
      requested_size = size;
   }

private:
   bool is_free;
   uint requested_size;
}
