#!/usr/bin/perl

use strict;
use warnings;
use encoding 'utf8'; # ä

use English qw( -no_match_vars ) ; # Avoids regex performance penalty
use Carp;

my $verbose = 0;

my $solver1   = shift;
my $solver2   = shift;
my @csp_files = @ARGV;

print "@csp_files\n";

my $error_count   = 0;
my $success_count = 0;
my $csp_count     = 0;

foreach my $csp_file (@csp_files) {
    print "Execute $solver1 $csp_file.\n" if $verbose;
    
    my $time_cuser = (times)[2];
    my @output1 = `$solver1 $csp_file`;
    my $time1 = (times)[2] - $time_cuser;
    
    my @result1 = ();
    foreach my $line (@output1) {
        #print $line;
        
        if ($line =~ /: (0|1)$/) {
            my $consistent_or_not = $1;
            push @result1, $consistent_or_not;
            
            #print "Result: $consistent_or_not\n";
        }
        else {
            #print "Ignore this line.\n";
        }

    }
    
    
    print "Execute $solver2 $csp_file.\n" if $verbose;
    
    $time_cuser = (times)[2];
    my @output2 = `$solver2 $csp_file`;
    my $time2 = (times)[2] - $time_cuser;
    
    my @result2 = ();
    foreach my $line (@output2) {
        #print $line;
        
        if ($line =~ /: (0|1)$/) {
            my $consistent_or_not = $1;
            push @result2, $consistent_or_not;
            
            #print "Result: $consistent_or_not\n";
        }
        else {
            #print "Ignore this line.\n";
        }
    }
    
    print "Compare results.\n" if $verbose;
    foreach my $result1 (@result1) {
        my $result2 = shift @result2;
        
        if (! defined $result2) {
            croak "result2 not defined!";
        }
        
        if ($result1 == $result2) {
            print ".";
            $success_count++;
        }
        else {
            print "!";
            $error_count++;
        }
        $csp_count++;
    }
    
    printf ' %.2f s, %.2f s', $time1, $time2;
    print "\n";
}

print "Passed $success_count/$csp_count tests.\n";
