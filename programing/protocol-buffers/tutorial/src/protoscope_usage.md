
refer: https://github.com/protocolbuffers/protoscope


```
ubuntu@VM-0-16-ubuntu:~/github/mac-utils/programing/protocol-buffers/tutorial/src$ ./encode_decode Encode(SerializeToZeroCopyStream) ok, uLen(9)
strHex(0a070a056765727279)
Decode(ParseFromArray) ok
ubuntu@VM-0-16-ubuntu:~/github/mac-utils/programing/protocol-buffers/tutorial/src$ echo "0a070a056765727279" | xxd -r -ps | protoscope1: {1: {"gerry"}}
ubuntu@VM-0-16-ubuntu:~/github/mac-utils/programing/protocol-buffers/tutorial/src$ echo "0a070a056765727279" | xxd -r -ps | protoscope -descriptor-set ../proto/addressbook.proto.pb -message-type tutorial.AddressBook -print-field-names
1: {            # people
  1: {"gerry"}  # name
}
ubuntu@VM-0-16-ubuntu:~/github/mac-utils/programing/protocol-buffers/tutorial/src$ xxd ../proto/addressbook.proto.pb 00000000: 0af6 030a 1161 6464 7265 7373 626f 6f6b  .....addressbook
00000010: 2e70 726f 746f 1208 7475 746f 7269 616c  .proto..tutorial
00000020: 222e 0a04 496e 666f 120e 0a01 6118 0120  "...Info....a.. 
00000030: 0128 0548 0052 0161 120e 0a01 6218 0220  .(.H.R.a....b.. 
00000040: 0128 0948 0052 0162 4206 0a04 7061 7261  .(.H.R.bB...para
00000050: 22e5 020a 0650 6572 736f 6e12 120a 046e  "....Person....n
00000060: 616d 6518 0120 0128 0952 046e 616d 6512  ame.. .(.R.name.
00000070: 0e0a 0269 6418 0220 0128 0552 0269 6412  ...id.. .(.R.id.
00000080: 140a 0565 6d61 696c 1803 2001 2809 5205  ...email.. .(.R.
00000090: 656d 6169 6c12 340a 0670 686f 6e65 7318  email.4..phones.
000000a0: 0420 0328 0b32 1c2e 7475 746f 7269 616c  . .(.2..tutorial
000000b0: 2e50 6572 736f 6e2e 5068 6f6e 654e 756d  .Person.PhoneNum
000000c0: 6265 7252 0670 686f 6e65 7312 2e0a 046d  berR.phones....m
000000d0: 6574 6118 0520 0328 0b32 1a2e 7475 746f  eta.. .(.2..tuto
000000e0: 7269 616c 2e50 6572 736f 6e2e 4d65 7461  rial.Person.Meta
000000f0: 456e 7472 7952 046d 6574 611a 550a 0b50  EntryR.meta.U..P
00000100: 686f 6e65 4e75 6d62 6572 1216 0a06 6e75  honeNumber....nu
00000110: 6d62 6572 1801 2001 2809 5206 6e75 6d62  mber.. .(.R.numb
00000120: 6572 122e 0a04 7479 7065 1802 2001 280e  er....type.. .(.
00000130: 321a 2e74 7574 6f72 6961 6c2e 5065 7273  2..tutorial.Pers
00000140: 6f6e 2e50 686f 6e65 5479 7065 5204 7479  on.PhoneTypeR.ty
00000150: 7065 1a37 0a09 4d65 7461 456e 7472 7912  pe.7..MetaEntry.
00000160: 100a 036b 6579 1801 2001 2809 5203 6b65  ...key.. .(.R.ke
00000170: 7912 140a 0576 616c 7565 1802 2001 2809  y....value.. .(.
00000180: 5205 7661 6c75 653a 0238 0122 2b0a 0950  R.value:.8."+..P
00000190: 686f 6e65 5479 7065 120a 0a06 4d4f 4249  honeType....MOBI
000001a0: 4c45 1000 1208 0a04 484f 4d45 1001 1208  LE......HOME....
000001b0: 0a04 574f 524b 1002 2237 0a0b 4164 6472  ..WORK.."7..Addr
000001c0: 6573 7342 6f6f 6b12 280a 0670 656f 706c  essBook.(..peopl
000001d0: 6518 0120 0328 0b32 102e 7475 746f 7269  e.. .(.2..tutori
000001e0: 616c 2e50 6572 736f 6e52 0670 656f 706c  al.PersonR.peopl
000001f0: 6562 0670 726f 746f 33                   eb.proto3
ubuntu@VM-0-16-ubuntu:~/github/mac-utils/programing/protocol-buffers/tutorial/src$ protoscope ../proto/addressbook.proto.pb > ../proto/addressbook.proto.pb.txtubuntu@VM-0-16-ubuntu:~/github/mac-utils/programing/protocol-buffers/tutorial/src$ protoscope -s ../proto/addressbook.proto.pb.txt > ../proto/addressbook.proto.pb2
ubuntu@VM-0-16-ubuntu:~/github/mac-utils/programing/protocol-buffers/tutorial/src$ echo "0a070a056765727279" | xxd -r -ps | protoscope -descriptor-set ../proto/addressbook.proto.pb2 -message-type tutorial.AddressBook -print-field-names
1: {            # people
  1: {"gerry"}  # name2
}
```
