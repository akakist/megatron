#!/usr/bin/perl
print "BEGIN;\n";
for($i=0;$i<10000;$i++)
{
    print "insert into tl_invites_create_sector (invitation_code) values ($i);\n"
}

for($i=0;$i<10000;$i++)
{
#    print "insert into tl_invites_reg (invitation_code,coop_sector_id) values ($i,19);\n"
}
print "COMMIT;\n";
